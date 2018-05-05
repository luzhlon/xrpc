target 'xrpc'
    set_kind 'shared'

    add_headers '*.h'
    -- add_headers '../deps/xval/src/*.h'
    add_files '*.cpp'
    add_files '../deps/xval/src/*.cpp'
    add_files '../deps/websocket-cpp/src/*.cpp'

    add_includedirs('../deps/websocket-cpp/src',
                    '../deps/xval/src')

    add_linkdirs('$(buildir)')
    -- add_links('websocket')

    add_defines 'DLL_EXPORT'

    if is_mode 'debug' then
        add_cxxflags '/MDd'
    end
