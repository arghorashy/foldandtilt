cmake_minimum_required(VERSION 2.6)


#Déclaration du projet
project(FoldnTilt)

#Définir où le produit final sera mis
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

#Chercher OpenCV
FIND_PACKAGE( OpenCV REQUIRED )


#Génération de la liste des fichiers sources
file(

        GLOB_RECURSE

        source_files

        ../FoldAndTilt/main.cpp

        # Could do something here like src/*

)


#Déclaration de l'exécutable
add_executable( ${PROJECT_NAME} ${source_files} )

#Configuration de l'édition de liens
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

#Get cascade files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory bin/${CMAKE_BUILD_TYPE}/assets/cascades)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ../assets/cascades bin/${CMAKE_BUILD_TYPE}/assets/cascades)
