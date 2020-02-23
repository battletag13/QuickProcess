{
  "targets": [{
    "target_name": "module",
    'include_dirs': [
      '.',
      '/usr/local/include',
      '/usr/local/include/opencv4'
    ],
    'cflags': [
      '-std=c++2a'
    ],
    'cflags_cc!': [ '-fno-rtti', '-fno-exceptions' ],
    'cflags!': [ '-fno-exceptions' ],
    'conditions': [
      ['OS=="mac"', {
        'xcode_settings': {
          'GCC_ENABLE_CPP_RTTI': 'YES',
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
        }
      }]
    ],
    'link_settings': {
      'libraries': [
        # '-L/Users/matthewlin/Desktop/SyncWGoogle/PortfolioProjects/QuickProcessElectron/lib',
        '-Wl,-rpath,@loader_path/../lib',
        '-lopencv_core.4.2',
        '-lopencv_imgproc.4.2',
        '-lopencv_imgcodecs.4.2'
      ],
    },
    "sources": [ "image-process.cc" ]
  }]
}
