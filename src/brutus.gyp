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
        'alloc.cc',
        'arena.cc',
        'ast.cc',
        'brutus.cc',
        'compiler.cc',
        'lexer.cc',
        'list.cc',
        'name.cc',
        'parser.cc',
        'phases.cc',
        'scopes.cc',
        'stopwatch.cc',
        'streams.cc',
        'symbols.cc',
        'types.cc'
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
