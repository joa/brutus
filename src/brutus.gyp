{
  'includes': ['../build/common.gypi'],
  'targets': [
    {
      'target_name': 'brutus',
      'type': 'executable',
      'dependencies': [],
      'defines': [
        'DEFINE_FOO',
        'DEFINE_A_VALUE=value',
      ],
      'include_dirs': [],
      'sources': [
       'brutus.cc',
       'lexer.cc',
       'parser.cc',
       'streams.cc',
       'stopwatch.cc',
      ],
      'conditions': [
        ['OS=="linux"', {
          'defines': [
            'LINUX_DEFINE',
          ],
          'include_dirs': [
            'include/linux',
          ],
        }],
        ['OS=="win"', {
          'defines': [
            'WINDOWS_SPECIFIC_DEFINE',
          ],
        }, { # OS != "win",
          'defines': [
            'NON_WINDOWS_DEFINE',
          ],
        }]
      ],
    }],
  }
