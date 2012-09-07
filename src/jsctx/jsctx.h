#pragma once

#include "../required.h"
#include "../util/log.h"
#include "../util/log_js.h"

namespace ion
{
	class jsctx
	{
	public:
		jsctx() : 
			locker(),
			hscope(),
			context(v8::Context::New(NULL, v8::ObjectTemplate::New())),
			contextScope(context),
			global(context->Global())
		{
			//Expose logging class
			exportGlobalObject("log", 
				[=](v8::Local<v8::Template> proto_t) {
					proto_t->Set("dbg", v8::FunctionTemplate::New(log_js::dbg));
					proto_t->Set("info", v8::FunctionTemplate::New(log_js::info));
				}, NULL);
			
		}

		~jsctx()
		{
		}

		void exportGlobalObject(const std::string& name, std::function<void(v8::Local<v8::Template>)> lambda, void* thisptr = 0)
		{
			v8::Handle<v8::FunctionTemplate> templ = v8::FunctionTemplate::New();
			v8::Local<v8::ObjectTemplate> objInst = templ->InstanceTemplate();
			objInst->SetInternalFieldCount(1);
			v8::Local<v8::Template> proto_t = templ->PrototypeTemplate();
			lambda(proto_t);
			v8::Handle<v8::Function> ctor = templ->GetFunction();
			v8::Handle<v8::Object> obj = ctor->NewInstance();
			obj->SetInternalField(0, v8::External::New(0));
			global->Set(v8::String::New(name.c_str()), obj);
		}

		static void setupTryCatch(v8::TryCatch & tc)
		{
			tc.SetVerbose(true);
			tc.SetCaptureMessage(true);
		}

		void reportException(v8::TryCatch* try_catch)
        {
            if( !try_catch ) return;
            v8::HandleScope hsc;
            v8::String::Utf8Value const excUtf(try_catch->Exception());
#define TOCSTR(X) (*X ? *X : "<exception string conversion failed!>")
            const char* excCstr = TOCSTR(excUtf);
            v8::Handle<v8::Message> const & message( try_catch->Message() );
            std::ostringstream os;
            os << "V8Shell Exception Reporter: ";
            if (message.IsEmpty())
            {
                // V8 didn't provide any extra information about this error; just
                // print the exception.
                os << excCstr << '\n';
            }
            else
            {
                // output (filename):(line number): (message)...
                int linenum = message->GetLineNumber();
                os << *v8::String::Utf8Value(message->GetScriptResourceName()) << ':'
                   << std::dec << linenum << ": "
                   << excCstr << '\n';
                // output source code line...
                os << *v8::String::AsciiValue(message->GetSourceLine()) << '\n';
                // output decoration pointing to error location...
                int start = message->GetStartColumn();
                for (int i = 0; i < start; i++) {
                    os << '-';
                }
                int end = message->GetEndColumn();
                for (int i = start; i < end; i++) {
                    os << '^';
                }
                os << '\n';
            }
            std::string const & str( os.str() );
			dbglogn(str);
            //reporter( str.c_str() );
#undef TOCSTR
        }

		v8::Handle<v8::Value> executeString(v8::Handle<v8::String> const & source,
                                           v8::Handle<v8::Value> name,
                                           std::ostream * out = NULL )
        {
            v8::HandleScope scope;
            v8::TryCatch tc;
            setupTryCatch(tc);
            v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
            if( script.IsEmpty())//tc.HasCaught())
            {
                // Report errors that happened during compilation.
                reportException(&tc);
                return scope.Close(tc.ReThrow());
                //return v8::Handle<v8::Value>();
            }
            else
            {
                v8::Handle<v8::Value> const & result( script->Run() );
                if( tc.HasCaught())//(result.IsEmpty())
                {
                    //executeThrew = true;
                    reportException(&tc);
                    //return v8::Handle<v8::Value>();
                    return scope.Close(tc.ReThrow());
                }
                else
                {
                    if (out && !result.IsEmpty())
                    {
                        (*out) << *v8::String::Utf8Value(result) << '\n';
                    }
                    return scope.Close(result);
                }
            }
        }

        v8::Handle<v8::Value> executeString(std::string const & source,
                                           std::string const & name,
                                           std::ostream * resultGoesTo )
        {
            v8::HandleScope scope;
            v8::Local<v8::String> const & s( v8::String::New( source.c_str(), static_cast<int>(source.size()) ) );
            v8::Local<v8::String> const & n( v8::String::New( name.c_str(), static_cast<int>(name.size()) ) );
            return scope.Close(executeString( s, n, resultGoesTo ));
        }

        v8::Handle<v8::Value> executeString(std::string const & source )
        {
            return executeString(source, "executeString()", 0);
        }

        v8::Handle<v8::Value> executeString(v8::Handle<v8::String> source )
        {
            return executeString(source, v8::String::New("executeString()"), 0);
        }

		v8::Handle<v8::Context> Context()
        {
            return context;
        }

		v8::Handle<v8::Object> Global()
        {
            return global;
        }

		jsctx & operator()( char const * name, v8::Handle<v8::Function> const & f )
        {
            global->Set( v8::String::New(name), f );
            return *this;
        }

        jsctx & operator()( char const * name, v8::Handle<v8::FunctionTemplate> const & f )
        {
            return operator()( name, f->GetFunction() );
        }

        jsctx & operator()( char const * name, v8::InvocationCallback const f )
        {
            return operator()( name, v8::FunctionTemplate::New(f) );
        }


	private:
		v8::Locker locker;
		v8::HandleScope hscope;
		v8::Handle<v8::Context> context;
		v8::Handle<v8::Object> global;
		v8::Context::Scope contextScope;
	};
}