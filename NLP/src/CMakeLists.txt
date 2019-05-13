#
# Creator by Sven
#
add_library(NLP SHARED
        DomainProxy.cpp
	  	MessageInterpreter.cpp
		NLPDomain.cpp
		NLPMessage.cpp
		DomainProcessor.cpp
		DomainRouter.cpp
		DomainSequencer.cpp)

target_include_directories(NLP PUBLIC
        "${NLP_SOURCE_DIR}/include"
	"${JSONCPP_INCLUDE_DIR}")

target_link_libraries(NLP AICommon)

# install target
asdk_install()

