env = Environment()
Export('env')

buildDir = 'builds'

# https://bitbucket.org/scons/scons/wiki/UsingBuildDir
SConscript(
    'src/SConstruct',
    variant_dir = buildDir,
    duplicate = 0
)