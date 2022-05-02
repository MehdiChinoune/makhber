
execute_process( COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
  OUTPUT_VARIABLE _Python3_EXT_SUFFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set( makhber_SIP_HDR ${CMAKE_CURRENT_BINARY_DIR}/sip.h )
set_source_files_properties( ${makhber_SIP_HDR} PROPERTIES GENERATED ON )
if( QT_VERSION_MAJOR VERSION_EQUAL 6 )
  set( sip_abi 13 )
else()
  set( sip_abi 12 )
endif()
add_custom_command(
  OUTPUT ${makhber_SIP_HDR}
  COMMAND ${SIP_Module_EXECUTABLE}
    --sip-h
    --abi-version ${sip_abi}
    ${CMAKE_CURRENT_SOURCE_DIR}/makhber${QT_VERSION_MAJOR}.sip
)
add_custom_target( generate_SIP_HDR DEPENDS ${makhber_SIP_HDR} )

get_target_property( QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION )

if( MAKHBER_EMBED_PYTHON_MODULE )
  set( makhber_PyQt_HDR ${SIP_BUILD_PATH}/sipAPImakhber.h )
  set( makhber_PyQt_SRCS
    ${SIP_BUILD_PATH}/sipmakhbercmodule.cpp
    ${SIP_BUILD_PATH}/sipmakhberApplicationWindow.cpp
    ${SIP_BUILD_PATH}/sipmakhberGraph.cpp
    ${SIP_BUILD_PATH}/sipmakhberArrowMarker.cpp
    ${SIP_BUILD_PATH}/sipmakhberImageMarker.cpp
    ${SIP_BUILD_PATH}/sipmakhberLegend.cpp
    ${SIP_BUILD_PATH}/sipmakhberMultiLayer.cpp
    ${SIP_BUILD_PATH}/sipmakhberTable.cpp
    ${SIP_BUILD_PATH}/sipmakhberMatrix.cpp
    ${SIP_BUILD_PATH}/sipmakhberMyWidget.cpp
    ${SIP_BUILD_PATH}/sipmakhberScriptEdit.cpp
    ${SIP_BUILD_PATH}/sipmakhberNote.cpp
    ${SIP_BUILD_PATH}/sipmakhberPythonScript.cpp
    ${SIP_BUILD_PATH}/sipmakhberPythonScripting.cpp
    ${SIP_BUILD_PATH}/sipmakhberFolder.cpp
    ${SIP_BUILD_PATH}/sipmakhberFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberExponentialFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberTwoExpFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberThreeExpFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberSigmoidalFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberGaussAmpFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberLorentzFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberNonLinearFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberPluginFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberMultiPeakFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberPolynomialFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberLinearFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberGaussFit.cpp
    ${SIP_BUILD_PATH}/sipmakhberFilter.cpp
    ${SIP_BUILD_PATH}/sipmakhberDifferentiation.cpp
    ${SIP_BUILD_PATH}/sipmakhberIntegration.cpp
    ${SIP_BUILD_PATH}/sipmakhberInterpolation.cpp
    ${SIP_BUILD_PATH}/sipmakhberSmoothFilter.cpp
    ${SIP_BUILD_PATH}/sipmakhberFFTFilter.cpp
    ${SIP_BUILD_PATH}/sipmakhberFFT.cpp
    ${SIP_BUILD_PATH}/sipmakhberCorrelation.cpp
    ${SIP_BUILD_PATH}/sipmakhberConvolution.cpp
    ${SIP_BUILD_PATH}/sipmakhberDeconvolution.cpp
    ${SIP_BUILD_PATH}/sipmakhberAbstractAspect.cpp
    ${SIP_BUILD_PATH}/sipmakhberColumn.cpp
    ${SIP_BUILD_PATH}/sipmakhberQwtSymbol.cpp
    ${SIP_BUILD_PATH}/sipmakhberQwtPlotCurve.cpp
    ${SIP_BUILD_PATH}/sipmakhberQwtPlot.cpp
    ${SIP_BUILD_PATH}/sipmakhberGrid.cpp
    ${SIP_BUILD_PATH}/sipmakhberQList0100QDateTime.cpp
    ${SIP_BUILD_PATH}/sipmakhberQList0101Folder.cpp
    ${SIP_BUILD_PATH}/sipmakhberQList0101MyWidget.cpp
    ${SIP_BUILD_PATH}/sipmakhberQList0101QwtPlotCurve.cpp
  )
  set_source_files_properties( ${makhber_PyQt_HDR} ${makhber_PyQt_SRCS}
    TARGET_DIRECTORY libmakhber
    PROPERTIES GENERATED ON
  )
  configure_file( pyproject.toml.in pyproject.toml @ONLY )
  add_custom_command( OUTPUT ${makhber_PyQt_HDR} ${makhber_PyQt_SRCS}
    COMMAND ${SIP_Build_EXECUTABLE} --verbose --no-make --build-dir build
    DEPENDS "makhber${QT_VERSION_MAJOR}.sip" ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    COMMAND_EXPAND_LISTS
    COMMENT "Generating Makhber sip Sources...\n"
  )
  add_custom_target( generate_PyQt_source
    ALL
    DEPENDS ${makhber_PyQt_HDR} ${makhber_PyQt_SRCS}
  )
  target_sources( libmakhber PRIVATE ${makhber_PyQt_HDR} ${makhber_PyQt_SRCS} )
  add_dependencies( libmakhber generate_PyQt_source )
#[[
  python3_add_library( makhber_sip
    ${makhber_PyQt_HDR}
    ${makhber_PyQt_SRCS}
  )
  set_target_properties( makhber_sip PROPERTIES
    OUTPUT_NAME "makhber"
    PREFIX ""
    SUFFIX ${_Python3_EXT_SUFFIX}
  )
  add_dependencies( makhber_sip generate_PyQt_source )
  target_link_libraries( makhber_sip PRIVATE libmakhber )
  if( MULTI_CONFIG )
    target_include_directories( makhber_sip PUBLIC "${CMAKE_BINARY_DIR}/src/libmakhber_autogen/include_@CONFIG@" )
  else()
    target_include_directories( makhber_sip PUBLIC "${CMAKE_BINARY_DIR}/src/libmakhber_autogen/include" )
  endif()
  install( TARGETS makhber_sip
    COMPONENT Python
    LIBRARY DESTINATION ${PYTHON_INSTALL_PATH}
  )
]]
else()
  macro( get_lib_properties LIB LIB_NAME LIB_DIR LIB_INCLUDE_DIRS )
    get_target_property( IS_IMPORTED ${LIB} IMPORTED )
    if( WIN32 )
      get_target_property( _LIB_LOCATION ${LIB} IMPORTED_IMPLIB_RELEASE )
      if( NOT _LIB_LOCATION )
        get_target_property( _LIB_LOCATION ${LIB} IMPORTED_IMPLIB )
      endif()
    endif()
    if( NOT _LIB_LOCATION OR NOT WIN32 )
      get_target_property( _LIB_LOCATION ${LIB} LOCATION )
    endif()
    if( _LIB_LOCATION )
      get_filename_component( _LIB_NAME ${_LIB_LOCATION} NAME_WE )
      string( REGEX REPLACE "^lib" "" LIB_NAME ${_LIB_NAME} )
      get_filename_component( LIB_DIR ${_LIB_LOCATION} DIRECTORY )
    else()
      message( WARNING "Couldn't find location for ${LIB}" )
    endif()
    get_target_property( LIB_INCLUDE_DIRS ${LIB} INTERFACE_INCLUDE_DIRECTORIES )
    if( NOT LIB_INCLUDE_DIRS )
      message( WARNING "Couldn't find include directories for ${LIB}" )
      set( LIB_INCLUDE_DIRS "" )
    endif()
  endmacro()

  set( qt_libs Core Gui Widgets PrintSupport Svg )
  if( MAKHBER_SEARCH_FOR_UPDATES OR MAKHBER_DOWNLOAD_LINKS )
    list( APPEND qt_libs Network )
  endif()
  # Dependency of QwtPlot3D
  if( QT_VERSION_MAJOR VERSION_EQUAL 6 )
    find_package( Qt6OpenGLWidgets REQUIRED )
    list( APPEND qt_libs OpenGLWidgets )
  endif()
  foreach( lib ${qt_libs} )
    get_lib_properties( Qt${QT_VERSION_MAJOR}::${lib} LIB_NAME LIB_DIR LIB_INCLUDE_DIRS )
    if( NOT APPLE )
      list( PREPEND SIP_LINK_LIBS ${LIB_NAME} )
      list( PREPEND SIP_LINK_DIRS ${LIB_DIR} )
    endif()
    list( PREPEND SIP_INCLUDE_DIRS ${LIB_INCLUDE_DIRS} )
  endforeach()

  set( libs muparser::muparser Qwt::Qwt )
  foreach( lib ${libs} )
    get_lib_properties( ${lib} LIB_NAME LIB_DIR LIB_INCLUDE_DIRS )
    list( PREPEND SIP_LINK_LIBS ${LIB_NAME} )
    list( PREPEND SIP_LINK_DIRS ${LIB_DIR} )
    list( PREPEND SIP_INCLUDE_DIRS ${LIB_INCLUDE_DIRS} )
  endforeach()

  if( APPLE )
    set( libs ${GSL_CBLAS_LIBRARY} ${GSL_LIBRARY} )
  else()
    set( libs ${GSL_CBLAS_LIBRARY} ${GSL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} )
    list( PREPEND SIP_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} )
  endif()
  foreach( lib ${libs} )
    get_filename_component( _LIB_NAME ${lib} NAME_WE )
    string( REGEX REPLACE "^lib" "" LIB_NAME ${_LIB_NAME} )
    list( PREPEND SIP_LINK_LIBS ${LIB_NAME} )
    get_filename_component( LIB_DIR ${lib} DIRECTORY )
    if( LIB_DIR )
      list( PREPEND SIP_LINK_DIRS ${LIB_DIR} )
    endif()
  endforeach()
  list( PREPEND SIP_INCLUDE_DIRS ${GSL_INCLUDE_DIRS} )

  if( GL2PS_LIBRARY )
    get_filename_component( _LIB_NAME ${GL2PS_LIBRARY} NAME_WE )
    string( REGEX REPLACE "^lib" "" LIB_NAME ${_LIB_NAME} )
    list( PREPEND SIP_LINK_LIBS ${LIB_NAME} )
    get_filename_component( LIB_DIR ${GL2PS_LIBRARY} DIRECTORY )
    if( LIB_DIR )
      list( PREPEND SIP_LINK_DIRS ${LIB_DIR} )
    endif()
    list( PREPEND SIP_INCLUDE_DIRS ${GL2PS_INCLUDE_DIR} )
  endif()

  list( PREPEND SIP_LINK_LIBS
    makhber
    origin
    qwtplot3d
  )

  if( MULTI_CONFIG )
    if( BUILD_SHARED_LIBS )
      list( PREPEND SIP_LINK_DIRS
        ${CMAKE_BINARY_DIR}/bin/@CONFIG@
        ${CMAKE_BINARY_DIR}/lib/@CONFIG@
        ${CMAKE_BINARY_DIR}/3rdparty/liborigin/@CONFIG@
      )
    else()
      list( PREPEND SIP_LINK_DIRS
        ${CMAKE_BINARY_DIR}/src/@CONFIG@
        ${CMAKE_BINARY_DIR}/3rdparty/liborigin/@CONFIG@
        ${CMAKE_BINARY_DIR}/3rdparty/qwtplot3d/@CONFIG@
      )
    endif()
  else()
    if( BUILD_SHARED_LIBS )
      list( PREPEND SIP_LINK_DIRS
        ${CMAKE_BINARY_DIR}/bin
        ${CMAKE_BINARY_DIR}/lib
      )
    else()
      list( PREPEND SIP_LINK_DIRS
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR}/3rdparty/liborigin
        ${CMAKE_BINARY_DIR}/3rdparty/qwtplot3d
      )
    endif()
  endif()
  list( REMOVE_DUPLICATES SIP_LINK_DIRS )
  string( JOIN "\"\n,\"" SIP_LINK_DIRS ${SIP_LINK_DIRS} )

  string( JOIN "\"\n,\"" SIP_LINK_LIBS ${SIP_LINK_LIBS} )

  foreach( lib libmakhber qwtplot3d )
    get_target_property( INCLUDE_DIRS ${lib} INTERFACE_INCLUDE_DIRECTORIES )
    list( PREPEND SIP_INCLUDE_DIRS ${INCLUDE_DIRS} )
  endforeach()
  list( REMOVE_DUPLICATES SIP_INCLUDE_DIRS )
  string( JOIN "\"\n,\"" SIP_INCLUDE_DIRS ${SIP_INCLUDE_DIRS} )
  string( REPLACE "$<CONFIG>" "@CONFIG@" SIP_INCLUDE_DIRS ${SIP_INCLUDE_DIRS} )

  get_target_property( COMPILE_DEFINITIONS libmakhber INTERFACE_COMPILE_DEFINITIONS )
  list( PREPEND SIP_COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} )
  list( TRANSFORM SIP_COMPILE_DEFINITIONS REPLACE "\"" "\\\\\"" )
  string( JOIN "\"\n,\"" SIP_COMPILE_DEFINITIONS ${SIP_COMPILE_DEFINITIONS} )

  set( SIP_COMPILE_OPTIONS "${CMAKE_CXX_FLAGS}" )
  if( MINGW )
    set( SIP_COMPILE_OPTIONS "${SIP_COMPILE_OPTIONS} -Wno-attributes" )
  endif()

  set( PyQt_INCLUDE_DIRS ${PyQt${QT_VERSION_MAJOR}_INCLUDE_DIRS} )

  if( WIN32 )
    set( module_name makhber.pyd )
  else()
    set( module_name makhber${_Python3_EXT_SUFFIX} )
  endif()

  if( MULTI_CONFIG )
    configure_file( pyproject.toml.in pyproject_tmp.toml @ONLY )
    add_custom_target( pyproject_toml ALL
      BYPRODUCTS pyproject.toml
      COMMAND ${CMAKE_COMMAND} -DCONFIG=$<CONFIG> -P ${CMAKE_CURRENT_SOURCE_DIR}/configure_toml.cmake
      DEPENDS libmakhber
      COMMENT "Generating pyproject.toml file..."
    )
    set( module_file ${SIP_BUILD_PATH}/$<CONFIG>/makhber/${module_name} )
  else()
    configure_file( pyproject.toml.in pyproject.toml @ONLY )
    set( module_file ${SIP_BUILD_PATH}/${module_name} )
  endif()

  if( NOT MSVC )
    set( QMAKE_CXX "${CMAKE_CXX_COMPILER}" )
    set( QMAKE_LINK "${CMAKE_CXX_COMPILER}" )
  else()
    get_filename_component( QMAKE_CXX "${CMAKE_CXX_COMPILER}" NAME )
    get_filename_component( QMAKE_LINK "${CMAKE_LINKER}" NAME )
  endif()

  if( MSVC AND NOT(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") )
    set( SIP_EXTRA_OPTIONS --qmake-setting QMAKE_CXXFLAGS+="/MP" )
  else()
    include( ProcessorCount )
    ProcessorCount( ncpu )
    set( SIP_EXTRA_OPTIONS --jobs ${ncpu} )
  endif()

  add_custom_command( OUTPUT ${module_file}
    COMMAND ${SIP_Build_EXECUTABLE} --verbose
      ${SIP_EXTRA_OPTIONS}
      --qmake-setting QMAKE_CXX=${QMAKE_CXX}
      --qmake-setting QMAKE_LINK=${QMAKE_LINK}
      --qmake-setting "CONFIG += c++${CMAKE_CXX_STANDARD} strict_c++"
      --qmake-setting $<IF:$<CONFIG:Debug>,"CONFIG+=debug","CONFIG+=release">
      --qmake-setting CONFIG+=$<$<BOOL:${CMAKE_INTERPROCEDURAL_OPTIMIZATION}>:"ltcg">
      --build-dir $<IF:$<BOOL:${MULTI_CONFIG}>,"build/$<CONFIG>","build">
    DEPENDS "makhber${QT_VERSION_MAJOR}.sip" libmakhber ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    COMMAND_EXPAND_LISTS
    COMMENT "Building Makhber Python Module..."
  )
  add_custom_target( makhber_sip
    ALL
    DEPENDS ${module_file}
  )
  if( MULTI_CONFIG )
    add_dependencies( makhber_sip pyproject_toml )
  endif()
  install( FILES ${module_file}
    COMPONENT Python
    DESTINATION ${PYTHON_INSTALL_PATH}
  )
endif()
