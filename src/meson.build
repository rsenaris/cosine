project( 'cosine'
       , 'cpp'
       , version : 'v0.0.0'
       , default_options : [ 'buildtype=debugoptimized'
                           , 'debug=true'
                           , 'optimization=2'
                           , 'cpp_std=c++17' # CAN'T GO TO C++20 DUE TO HIGHFIVE
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

hdf5   = dependency( 'hdf5'
                   , language: 'cpp'
                   , required   : true
                   )


high5 = dependency( 'HighFive'
                  , method  : 'cmake'
                  , required: true)

cosine_lib_deps     = [nain4, geant4, hdf5, high5]
cosine_lib_include  = include_directories('.')
cosine_lib_files    = [
 'actions/store_volume_crossing',
 'actions/event',
 'actions/tracking',
 'core/config',
 'core/messenger',
 'core/symbols',
 'core/types',
 'generators/geantinos',
 'generators/generic',
 'generators/pointlike',
 'generators/position',
 'generators/selector',
 'generators/scalar',
 'materials/LXe',
 'materials/ptfe',
 'materials/silicon',
 'materials/steel',
 'geometry/calibration_belt',
 'geometry/pcolina',
 'geometry/mesh',
 'geometry/sipm_array',
 'geometry/wire_array',
 'persistency/hdf5_types',
 'persistency/hdf5_writer',
 'persistency/manager',
 'sensitive/active',
 'sensitive/sipm',
]
cosine_lib_includes = ['core/common.hh', 'core/utils.hh']
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

install_headers(cosine_lib_includes, preserve_path: true)

pkg = import('pkgconfig')
pkg.generate( cosine_lib
            , description: 'Nain4 simulation of the COLINA detector'
            , name       : 'cosine'
            , filebase   : 'cosine'
            )
