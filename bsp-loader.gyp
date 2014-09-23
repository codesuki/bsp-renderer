{
    'make_global_settings': [
	['CXX','/usr/bin/clang++'],
	['LINK','/usr/bin/clang++'],
    ],
    'target_defaults': {
	'xcode_settings': {
            'OTHER_CFLAGS': [
		'-std=c++11',
		'-g',
		'-D_THREAD_SAFE',
	    ],
	    'OTHER_LDFLAGS': [
		'-L/usr/local/lib',
		'-lSDLmain',
		'-Wl,-framework,Cocoa',
	    ],
	},
	'cflags': [
            '-std=c++11',
        ],
	'product_dir': '../../resources/'
    },
    'targets': [
	{
	    'target_name': 'osx',
	    'product_name': 'bsp-renderer',
            'type': 'executable',
            'sources': [
		'src/bot_input_component.cpp',
		'src/shader_loader.cpp',
		'src/entity.cpp',
		'src/texture_loader.cpp',
		'src/font.cpp',
		'src/world.cpp',
		'src/logger.cpp',
		'src/bezier.cpp',
		'src/model.cpp',
		'src/bsp.cpp',
		'src/player_animation_component.cpp',
		'src/frustum.cpp',
		'src/player_input_component.cpp',
		'src/input.cpp',
		'src/player_physics_component.cpp',
		'src/main.cpp',
		'src/q3_shader.cpp',
		'src/messenger.cpp',
		'src/renderer.cpp',
		'src/shader.cpp',
            ],
	    'include_dirs': [
		'/usr/local/include',
		'/usr/local/include/SDL2'
            ],
	    'ldflags': [
		'-L/usr/local/lib',
            ],
	    'libraries': [
		'-framework OpenGL',
		'-lSDL2',
		'-lSDL2_image',
		'-lGlew',
		'-lboost_system',
		'-lboost_filesystem',
	    ],
	}],
}
