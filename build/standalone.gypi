{
  'variables': {
    'werror%': '-Werror',
  },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'cflags': [ '-g', '-O0' ],
      },
    },
  },
  'conditions': [
    ['OS=="linux"', {
      'target_defaults': {
        'cflags': ['-std=c++0x', '-Wall', '<(werror)', '-W', '-fno-rtti', '-fno-exceptions', '-pthread', '-pedantic'],
        'ldflags': ['-pthread'],
      },
    }], #'OS=="linux"'
  ],
}
