# -*- mode: CMake; cmake-tab-width: 4; -*-

include(FindPerl)

# "make pdf" creates the manual in PDF form. The other of the targets that now
# follow do the same for other formats
add_custom_target(pdf)
add_custom_target(docx)
add_custom_target(odt)
add_custom_target(epub)
add_custom_target(html5 ALL)

function(generate_wxmaxima_documentation BASENAME FORMAT PANDOC_TEXSELECT_OPTION)
    set(FILEEXTENSION "${FORMAT}")
    # for html5 format set the file extension to .html
    if(FILEEXTENSION STREQUAL "html5")
        set(FILEEXTENSION "html")
    endif()
    add_custom_command(
        OUTPUT ${BASENAME}.${FORMAT}
        DEPENDS ${BASENAME}.md
        COMMAND ${PANDOC} ${BASENAME}.md -t ${FORMAT} -o ${BASENAME}.${FILEEXTENSION} ${PANDOC_ARGS} ${PANDOC_TEXSELECT_OPTION}
        COMMENT "Generating ${BASENAME}.${FORMAT}")
    add_custom_target(${BASENAME}.${FORMAT}.stamp DEPENDS ${BASENAME}.${FORMAT})
    add_dependencies(${FORMAT} ${BASENAME}.${FORMAT}.stamp)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.${FORMAT} DESTINATION share/doc/wxmaxima OPTIONAL)
endfunction()



# Create a list of the common part of the name of all the translated manuals.
# The "list(TRANSFORM" type of commands only is available in very new cmake
# versions and therefore (in 2019) of only limited use so we'll have to do
# this by hand.
set(BASENAMES "wxmaxima")
file(GLOB POFILES ${CMAKE_SOURCE_DIR}/locales/manual/*.po)
file(GLOB MANUAL_DEPENDS ${CMAKE_SOURCE_DIR}/locales/manual/*.po wxmaxima.md)
foreach(POFILE ${POFILES})
    string(REGEX REPLACE ".*/(.*).po$" "wxmaxima.\\1" BASENAME ${POFILE})
    list(APPEND BASENAMES ${BASENAME})
endforeach()

# Find a suitable LaTeX installation
find_program(LATEX NAMES xelatex lualatex)
if(NOT LATEX)
    message(WARNING "Xelatex or Lualatex not found. PDF documentation can not be converted from Markdown.")
endif()

find_program(PO4A_TRANSLATE po4a-translate)
if(NOT PO4A_TRANSLATE)
    message(WARNING "po4a-translate not found. Localized documentation can not be converted from Markdown.")
endif()

if (NOT PO4A_TRANSLATE)
    message(STATUS "No po4a-translate binary")
    if(NOT PO4A_TRANSLATE)
        find_package(Perl)
	 if(PERL_FOUND)
	    message(STATUS "Found perl")
	    find_file(PO4A_TRANSLATE po4a-translate)
	    if(PO4A_TRANSLATE)
		message(STATUS "Found po4a-translate")
		get_filename_component(PO4A_TRANSLATE_DIR ${PO4A_TRANSLATE} DIRECTORY)
		set(PO4A-TRANSLATE ${PERL_EXECUTABLE} -I${PO4A_TRANSLATE_DIR}/lib ${PO4A_TRANSLATE})
		message(STATUS "PO4A_TRANSLATE = ${PO4A_TRANSLATE}")
	    endif()
	endif()
    endif()
endif()

# Generate the manuals
find_program(PANDOC pandoc)
if(NOT PANDOC)
    message(WARNING "Pandoc not found. Documentation can not be converted from Markdown.")
endif()

if(PANDOC AND PO4A_TRANSLATE)
    execute_process(
        COMMAND ${PANDOC} --version
        OUTPUT_VARIABLE PANDOC_VERSION_TEXT
	)
    # get pandoc major version number, if >2 use --pdf-engine else --latex-engine (they changed the name of the command line option)
    string(REGEX REPLACE "^pandoc ([0-9]).*$" "\\1"  PANDOC_VERSION ${PANDOC_VERSION_TEXT})
    if(PANDOC_VERSION LESS_EQUAL 1)
        message(STATUS "pandoc version < 2 found - using option --latex-engine=${LATEX}")
        set(PANDOC_TEXSELECT_OPTION "--latex-engine=${LATEX}")
    else()
        message(STATUS "pandoc version >= 2 found - using option --pdf-engine=${LATEX}")
        set(PANDOC_TEXSELECT_OPTION "--pdf-engine=${LATEX}")
    endif()
    set(PANDOC_ARGS --number-sections --table-of-contents --standalone --css=wxmaxima.css --metadata title="wxMaxima")
    if(NOT LATEX)
        message(STATUS "lualatex or xelatex required for building the pdf manual.")
    endif()
    file(GLOB IMAGEFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.png)
    file(COPY ${IMAGEFILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/wxmaxima.md DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    foreach(BASENAME ${BASENAMES})
	string(REGEX REPLACE "wxmaxima.(.*)$" "\\1" LANG ${BASENAME})
	string(REGEX REPLACE "wxmaxima$" "" LANG ${LANG})

	if(PO4A_TRANSLATE)
	    if(NOT BASENAME STREQUAL "wxmaxima")
		add_custom_command(
		    OUTPUT ${BASENAME}.md
		    DEPENDS ${CMAKE_SOURCE_DIR}/locales/manual/${LANG}.po ${CMAKE_CURRENT_SOURCE_DIR}/wxmaxima.md
		    COMMAND ${PO4A_TRANSLATE} -k 0 -f text -m ${CMAKE_CURRENT_SOURCE_DIR}/wxmaxima.md -p ${CMAKE_SOURCE_DIR}/locales/manual/${LANG}.po -l ${BASENAME}.md -M UTF-8)
	    endif()
	endif()

	add_custom_command(OUTPUT ${BASENAME}.html
	    COMMAND generate_wxmaxima_documentation(${BASENAME} html5 "")
	    )
	add_custom_target(generate_wxmaxima_html_${BASENAME} DEPENDS ${MANUAL_DEPENDS})
	add_custom_command(OUTPUT ${BASENAME}.odt
	    COMMAND generate_wxmaxima_documentation(${BASENAME} odt "")
	    )
	add_custom_target(generate_wxmaxima_odt_${BASENAME} DEPENDS ${MANUAL_DEPENDS})
	add_custom_command(OUTPUT ${BASENAME}.docx
	    COMMAND generate_wxmaxima_documentation(${BASENAME} docx "")
	    )
	add_custom_target(generate_wxmaxima_docx_${BASENAME} DEPENDS ${MANUAL_DEPENDS})
	add_custom_command(OUTPUT ${BASENAME}.epub
	    COMMAND generate_wxmaxima_documentation(${BASENAME} epub "")
	    )
	add_custom_target(generate_wxmaxima_epub_${BASENAME} DEPENDS ${MANUAL_DEPENDS})

	add_custom_command(OUTPUT ${BASENAME}.pdf
	    COMMAND generate_wxmaxima_documentation(${BASENAME} pdf "${PANDOC_TEXSELECT_OPTION}")
	    )
	if(LATEX)
	    add_custom_target(generate_wxmaxima_pdf_${BASENAME} DEPENDS ${MANUAL_DEPENDS})
	endif()


	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wxmaxima.hhp.in ${BASENAME}.hhp)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.hhp DESTINATION share/doc/wxmaxima)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${BASENAME}.html DESTINATION share/doc/wxmaxima)
    endforeach()
else()
    foreach(BASENAME ${BASENAMES})
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${BASENAME}.html DESTINATION share/doc/wxmaxima)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wxmaxima.hhp.in ${BASENAME}.hhp)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.hhp DESTINATION share/doc/wxmaxima OPTIONAL)
    endforeach()
endif()

file(COPY wxmaxima.css DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB WXMAXIMA_ADDITIONAL_HELP_FILES *.jpg *.png *.css)
install(FILES ${WXMAXIMA_ADDITIONAL_HELP_FILES} DESTINATION share/doc/wxmaxima)


