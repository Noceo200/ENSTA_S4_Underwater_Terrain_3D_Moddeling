cmake_minimum_required(VERSION 2.6) # compatibilités de CMake
project(create_raster) # nom du projet : "main_exe"

set(CMAKE_BUILD_TYPE debug) # compilation en mode debug (mode debug = prent en compt eles assert et mode release = ne les prend pas en compte)
#set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++11") # options

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROJ REQUIRED proj)

# Indiquez l'emplacement du code source de VTK
#set(VTK_DIR /include/VTK-9.2.2)

# Trouvez la bibliothèque VTK
find_package(VTK REQUIRED)

file( # liste des fichiers à compiler
	GLOB_RECURSE # recherche récursive
	source_files # fichiers listés dans une variable source_files
	src/*.cpp # forme des fichiers à rechercher
)

add_executable( # création de l’exécutable binaire du projet
	${PROJECT_NAME} # contient le nom du binaire
	${source_files} # liste des fichiers compilés pour le linking
)


target_include_directories(${PROJECT_NAME} PRIVATE include)

# Incluez les fichiers d'en-tête de VTK dans votre projet
include_directories(${VTK_INCLUDE_DIRS})

# Incluez les fichiers d'en-tête de VTK dans votre projet
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${PROJ_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJ_INCLUDEDIR}
)
