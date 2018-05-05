
#include "xval_str.h"
#include "../src/xrpc.h"

using namespace xrpc;
using namespace xval;

class MyHandler : public RPCHandler {
public:
    static void onopen(RPCHandler& h) {
        h.addFunc("test"_x, [](RPCHandler& h, Tuple& args)->Value {
            return "RECEIVED";
        });
        h.addFunc("echo"_x, [](RPCHandler& h, Tuple& args)->Value {
            return &args;
        }),
        h.addFunc("callecho"_x, [](RPCHandler& h, Tuple& args)->Value {
            return h.call("echo", {1, 2, 3});
        }),
        cout << h.getHost() << " opened" << endl;
    }
    static void onclose(RPCHandler& h) {
        cout << h.getHost() << " closed" << endl;
    }
};

int main(int argc, char **argv) {
    // RPCServer server("127.0.0.1", 5333);
    WebSocketServer<MyHandler> server(5333);
    server.run();
    return 0;
}
