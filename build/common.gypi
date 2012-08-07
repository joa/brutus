{
  'variables': {
  },

  'target_defaults': {
    'configurations': {
      'Debug': {
        'defines': [
          'DEBUG'
        ],
        'conditions': [
          ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="netbsd"', {
            'cflags': [ '-Wall', '<(werror)', '-W', '-Wno-unused-parameter',
                        '-Wnon-virtual-dtor', '-Woverloaded-virtual' ],
          }],
        ],
      }, #Debug
      'Release': {
        'conditions': [
          ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="netbsd" \
            or OS=="android"', {
            'cflags!': [
              '-O2',
              '-Os',
            ],
            'cflags': [
              '-fdata-sections',
              '-ffunction-sections',
              '-O3',
            ],
          }], #OS=="*nix"
        ], #conditions
      }, # Release
    }, # configurations
  }, # target_defaults
}
