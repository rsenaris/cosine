project( 'cosine'
       , 'cpp'
       , version : 'v0.0.0'
       , default_options : [ 'buildtype=debugoptimized'
                           , 'debug=true'
                           , 'optimization=2'
                           , 'cpp_std=c++20'
                           , 'prefix=@0@/../install/cosine/'.format(meson.source_root())
                           , 'libdir=lib'
                           ]
       )

nain4  = dependency( 'nain4'
                   , method  : 'pkg-config'
                   , required: true
                   )

geant4 = dependency( 'Geant4'
                   , method     : 'cmake'
                   , required   : true
                   , components : ['ui_all', 'vis_all']
                   , modules    : nain4.get_variable(pkgconfig: 'geant4_modules').split(','))

cosine_lib_deps     = [nain4, geant4]
cosine_lib_include  = include_directories('.')
cosine_lib_files    = [
 'actions/store_volume_crossing',
 'generators/geantinos',
 'materials/LXe',
 'materials/ptfe',
 'materials/silicon',
 'materials/steel',
 'geometry/pcolina',
 'geometry/mesh',
 'geometry/sipm_array',
 'geometry/wire_array',
]
cosine_lib_includes = ['common.hh']
cosine_lib_sources  = []
foreach f : cosine_lib_files
  cosine_lib_includes += [f + '.hh']
  cosine_lib_sources  += [f + '.cc']
endforeach

geant4_include = geant4.get_variable(cmake    : 'Geant4_INCLUDE_DIRS')
nain4_include  =  nain4.get_variable(pkgconfig: 'includedir'         )


cosine_lib = shared_library( 'cosine'
                                   , cosine_lib_sources
                                   , include_directories: [cosine_lib_include, geant4_include]
                                   , dependencies       : cosine_lib_deps
                                   , install            : true
                                   )

cosine_exe = executable( 'cosine'
                               , ['cosine.cc']
                               , include_directories: [cosine_lib_include, nain4_include, geant4_include]
                               , dependencies       : cosine_lib_deps
                               , link_with          : cosine_lib
                               , install            : true
                               )

install_headers(cosine_lib_includes)

pkg = import('pkgconfig')
pkg.generate( cosine_lib
            , description: 'Nain4 simulation of the COLINA detector'
            , name       : 'cosine'
            , filebase   : 'cosine'
            )
