
target 'test'
    set_kind 'binary'

    add_headers '../src/*.h'
    add_files '*.cpp'

    add_includedirs('../deps/xval/src', '../deps/websocket-cpp/src')
    add_linkdirs('$(buildir)')
    add_links('websocket', 'xrpc')

    if is_mode 'debug' then
        add_cxxflags '/MTd'
    end

    add_deps 'xrpc'
