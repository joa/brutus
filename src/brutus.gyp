{
  'includes': ['../build/common.gypi'],
  'targets': [
    {
      'target_name': 'brutus',
      'type': 'executable',
      'dependencies': [],
      'defines': [],
      'include_dirs': [],
      'sources': [
        'ast.cc',
        'brutus.cc',
        'lexer.cc',
        'parser.cc',
        'stopwatch.cc',
        'streams.cc',
      ],
      'conditions': [
        ['OS=="linux"', {
          'defines': [
            'OS_LINUX',
          ],
          'include_dirs': [
            'include/linux',
          ],
        }],
        ['OS=="win"', {
          'defines': [
            'OS_WINDOWS',
          ],
        }, { # OS != "win",
          'defines': [],
        }]
      ],
    }],
  }
