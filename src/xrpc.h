#ifndef __XRPC_H__
#define __XRPC_H__

#include "websocket.hpp"

#include "xval.h"
#include "xval_val.h"
#include "xval_list.h"
#include "xval_dict.h"

namespace xrpc {
    using namespace xval;
    using namespace websocket;

    class RPCHandler;

    typedef Value (*Function)(RPCHandler& h, Tuple& args);
    typedef void (*OnOpen)(RPCHandler& h);
    typedef OnOpen OnClose;

    class EXPORT RPCHandler : public WebSocketHandler {
    public:
        static void onopen(RPCHandler& h);
        static void onmessage(RPCHandler& h);
        static void onerror(RPCHandler& h);
        static void onclose(RPCHandler& h);

    public:
        void setAttr(const Value& k, const Value& v) {
            _attr.dict().set(k, v);
        }
        Value getAttr(const Value& k) {
            return _attr.dict().get(k);
        }
        // send a pack 'call' or 'signal'
        Value invoke(const Value& func, const Value *argv, size_t argc, bool signal = false);
        // send a pack 'call'
        Value call(const Value& func, const Value& args) {
            return invoke(func, &args, 1);
        }
        Value call(const Value& func, initializer_list<Value> args) {
            return invoke(func, args.begin(), args.size());
        }
        // send a pack 'signal'
        void signal(const Value& func, const Value& args) {
            invoke(func, &args, 1, true);
        }

        void addFunc(const Value& fid, Function f) {
            _funs.set(fid, (void*)f);
        }

        Function getFunc(const Value& id) {
            auto f = _funs.get(id);
            return f.ispointer() ? (Function)f.pointer() : nullptr;
        }

        OnOpen onOpen = nullptr;
        OnClose onClose = nullptr;

    private:
        void handle_call();
        Value wait_return();
        // about the pack
        void parse_pack();
        bool send_pack();
        inline bool iscall() { return _pack.get(0); }
        inline bool isreturn() { return !iscall(); }
        // send a pack 'return'
        bool retn(const Value& msgid, const Value& args);

        Value _funs = Dict::New(8);
        Value _pack = List::New(8);     // pack's buffer of list
        Value _attr = Dict::New(0);     // session's attributes
        size_t _idnum = 1;              // the id counter
    };

    typedef WebSocketServer<RPCHandler> _RPCServerBase;

    class EXPORT RPCServer : public _RPCServerBase {
    public:
        RPCServer(const char *addr, unsigned short port);
        RPCServer(unsigned short port);

        RPCHandler& accept();
        bool run();
    };
}

#endif /* __XRPC_H__ */
