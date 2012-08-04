{
  'variables': {
    'werror%': '-Werror',
  },
  'conditions': [
    ['OS=="linux"', {
      'target_defaults': {
        'cflags': ['-std=c++11', '-Wall', '<(werror)', '-W', '-fno-rtti', '-fno-exceptions', '-pedantic', '-pthread'],
        'ldflags': ['-pthread'],
      }
    }], #'OS=="linux"'
  ],
}
