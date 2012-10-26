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
        'arena.cc',
        'ast.cc',
        'brutus.cc',
        'lexer.cc',
        'name.cc',
        'parser.cc',
        'scopes.cc',
        'stopwatch.cc',
        'streams.cc',
        'symbols.cc'
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
