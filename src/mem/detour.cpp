#include "detour.h"

namespace ion
{
	bool detour::apply()
	{
		if (m_bApplied) return false;
		//Create trampoline
		UINT trampSize = JMP_SIZE * 3;
		DWORD dwOldProtTramp;

		m_tramp = malloc(trampSize + 1);
		memset(m_tramp, 0, trampSize);

		VirtualProtect(m_tramp, trampSize + 1, PAGE_EXECUTE_READWRITE, &dwOldProtTramp);
		BYTE* trampCur = (BYTE*)m_tramp;

		//Initialize the DISASM structure
		DISASM srcDisasm;
		memset(&srcDisasm, 0, sizeof DISASM);

		srcDisasm.Archi = 32;
		srcDisasm.EIP = (UIntPtr)this->m_src;
		srcDisasm.VirtualAddr = (UIntPtr)this->m_src;	
		unsigned int InstrSize = 0;
		do
		{
			int len = Disasm(&srcDisasm);

			if (len == UNKNOWN_OPCODE)
			{
				return false;
			}

			//Relocate relative jumps //TODO: Other branch types
			if (srcDisasm.Instruction.BranchType == JmpType &&
				srcDisasm.Instruction.AddrValue != 0)
			{
				jmp(trampCur, (void*)srcDisasm.Instruction.AddrValue);
				trampCur += JMP_SIZE;
			}
			else
			{
				//Generic instruction
				memcpy(trampCur, (void*)srcDisasm.VirtualAddr, len);
				trampCur += len;
			}

			//Advance
			srcDisasm.EIP += len;
			srcDisasm.VirtualAddr += len;

			InstrSize += len;
		} while(InstrSize < JMP_SIZE);

		int OverridenSize = InstrSize;

		//Jump back to source
		jmp(trampCur, (BYTE*)m_src + InstrSize);
		trampCur += JMP_SIZE;

		DWORD dwOldProtSrc;
		VirtualProtect(m_src, JMP_SIZE, PAGE_READWRITE, &dwOldProtSrc);

		//Backup for later
		if (!m_orig.empty()) m_orig.clear();
		m_orig.resize(OverridenSize);
		m_orig = memmgr::readBytes(m_src, OverridenSize);

		//Write jump from src to detour
		jmp(m_src, m_dst);
		VirtualProtect(m_src, JMP_SIZE, dwOldProtSrc, &dwOldProtSrc);
		m_bApplied = true;
	}

	void detour::jmp(void* addy, void* target)
	{
		AsmJit::Assembler jit;
		jit.jmp(target);
		DWORD_PTR const stubSize = jit.getCodeSize();

		if (stubSize != JMP_SIZE)
			return;

		std::vector<BYTE> jmpBuf(stubSize);
		jit.relocCode(jmpBuf.data(), (DWORD_PTR)addy);
		
		memmgr::writeBytes((BYTE*)addy, jmpBuf);
	}

	void detour::remove()
	{
		if (!m_bApplied) return;
		memmgr::writeBytes(m_src, m_orig);
		m_bApplied = false;
	}
}