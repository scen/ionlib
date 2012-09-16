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

		_DInst dec[1];
		UINT decCnt = 0;

		_CodeInfo ci = {0};
		ci.code = m_src;
		ci.codeOffset = (_OffsetType)m_src;
		ci.codeLen = 20;
		ci.dt = Decode32Bits;
		ci.features = DF_NONE;
		
		unsigned int InstrSize = 0;
		do
		{
			distorm_decompose(&ci, dec, 1, &decCnt);
			if (dec->flags == FLAG_NOT_DECODABLE)
			{
				dbglogn("flag_not_decodable");
				return 0;
			}
			if (META_GET_FC(dec->meta) == FC_UNC_BRANCH || META_GET_FC(dec->meta) == FC_CND_BRANCH) //TODO add more branch types
			{
				if (dec->opcode == I_JMP)
				{
					dbglogn("JMP branch");
					if (dec->ops[0].type == O_PC)
					{
						//If you want target address use INSTRUCTION_GET_TARGET()
						//otherwise just read from imm.addr for relative offset
						jmp(trampCur, (void*)dec->imm.addr);
						trampCur += dec->size;
					}
					else
					{
						dbglogn("unknown JMP op 0");
						return 0;
					}
				}
				else
				{
					dbglogn("[warning] no implementation for branch type: " << GET_MNEMONIC_NAME(dec->opcode));
				}
			}
			else
			{
				//non branching instruction
				memcpy(trampCur, (void*)ci.codeOffset, dec->size);
				trampCur += dec->size;
			}
			ci.codeOffset = ci.nextOffset;
			InstrSize += dec->size;
		} while (InstrSize < JMP_SIZE);
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