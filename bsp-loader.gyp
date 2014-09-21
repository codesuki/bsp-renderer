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
		'src/BotInputComponent.cpp',
		'src/ShaderLoader.cpp',
		'src/Entity.cpp',
		'src/TextureLoader.cpp',
		'src/Font.cpp',
		'src/World.cpp',
		'src/Logger.cpp',
		'src/bezier.cpp',
		'src/Model.cpp',
		'src/bsp.cpp',
		'src/PlayerAnimationComponent.cpp',
		'src/frustum.cpp',
		'src/PlayerInputComponent.cpp',
		'src/input.cpp',
		'src/PlayerPhysicsComponent.cpp',
		'src/main.cpp',
		'src/Q3Shader.cpp',
		'src/messenger.cpp',
		'src/Renderer.cpp',
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
