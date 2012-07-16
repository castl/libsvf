env = Environment(CPPPATH = ['.'],
                  CXXFLAGS = ['-O3', '-mfpmath=sse', '-msse4', '-march=native',
                              '-Wall', '-g', '-std=c++0x',
                              '-fopenmp'],
                  LIBS=[],
                  LIBPATH=[],
                  LINKFLAGS=[])


testrunner = env.Program('testrunner', Glob('tests/*.cpp'),
                            LIBS=['cppunit'])
