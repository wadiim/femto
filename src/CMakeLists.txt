if(UNIX)
  set(OS unix)
elseif(WIN32)
  set(OS windows)
endif()

set(SOURCE_FILES
    ${OS}/utils.c
    ${OS}/term.c
    ${OS}/io.c
    buffer.c
    status.c
    femto.c
    utils.c
    main.c
    file.c
    line.c
    text.c
    move.c
    die.c
)

if(WIN32)
  set(SOURCE_FILES ${SOURCE_FILES} ${OS}/getline.c)
endif()

add_executable(femto ${SOURCE_FILES})
