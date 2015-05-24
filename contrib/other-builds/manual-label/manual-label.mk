##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=manual-label
ConfigurationName      :=Debug
WorkspacePath          := "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds"
ProjectPath            := "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hieu Hoang
Date                   :=19/05/2015
CodeLitePath           :="/Users/hieu/Library/Application Support/codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -dynamiclib -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="manual-label.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../../.. $(IncludeSwitch)../../../boost/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)boost_program_options $(LibrarySwitch)boost_filesystem $(LibrarySwitch)boost_system 
ArLibs                 :=  "boost_program_options" "boost_filesystem" "boost_system" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../../../boost/lib64 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/Applications/codelite.app/Contents/SharedSupport/
Objects0=$(IntermediateDirectory)/DeEn.cpp$(ObjectSuffix) $(IntermediateDirectory)/EnOpenNLPChunker.cpp$(ObjectSuffix) $(IntermediateDirectory)/EnPhrasalVerb.cpp$(ObjectSuffix) $(IntermediateDirectory)/LabelByInitialLetter.cpp$(ObjectSuffix) $(IntermediateDirectory)/Main.cpp$(ObjectSuffix) $(IntermediateDirectory)/ZonesAndWalls.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/DeEn.cpp$(ObjectSuffix): DeEn.cpp $(IntermediateDirectory)/DeEn.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label/DeEn.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DeEn.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DeEn.cpp$(DependSuffix): DeEn.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/DeEn.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/DeEn.cpp$(DependSuffix) -MM "DeEn.cpp"

$(IntermediateDirectory)/DeEn.cpp$(PreprocessSuffix): DeEn.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/DeEn.cpp$(PreprocessSuffix) "DeEn.cpp"

$(IntermediateDirectory)/EnOpenNLPChunker.cpp$(ObjectSuffix): EnOpenNLPChunker.cpp $(IntermediateDirectory)/EnOpenNLPChunker.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label/EnOpenNLPChunker.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EnOpenNLPChunker.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EnOpenNLPChunker.cpp$(DependSuffix): EnOpenNLPChunker.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EnOpenNLPChunker.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EnOpenNLPChunker.cpp$(DependSuffix) -MM "EnOpenNLPChunker.cpp"

$(IntermediateDirectory)/EnOpenNLPChunker.cpp$(PreprocessSuffix): EnOpenNLPChunker.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EnOpenNLPChunker.cpp$(PreprocessSuffix) "EnOpenNLPChunker.cpp"

$(IntermediateDirectory)/EnPhrasalVerb.cpp$(ObjectSuffix): EnPhrasalVerb.cpp $(IntermediateDirectory)/EnPhrasalVerb.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label/EnPhrasalVerb.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EnPhrasalVerb.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EnPhrasalVerb.cpp$(DependSuffix): EnPhrasalVerb.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EnPhrasalVerb.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EnPhrasalVerb.cpp$(DependSuffix) -MM "EnPhrasalVerb.cpp"

$(IntermediateDirectory)/EnPhrasalVerb.cpp$(PreprocessSuffix): EnPhrasalVerb.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EnPhrasalVerb.cpp$(PreprocessSuffix) "EnPhrasalVerb.cpp"

$(IntermediateDirectory)/LabelByInitialLetter.cpp$(ObjectSuffix): LabelByInitialLetter.cpp $(IntermediateDirectory)/LabelByInitialLetter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label/LabelByInitialLetter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LabelByInitialLetter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LabelByInitialLetter.cpp$(DependSuffix): LabelByInitialLetter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LabelByInitialLetter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LabelByInitialLetter.cpp$(DependSuffix) -MM "LabelByInitialLetter.cpp"

$(IntermediateDirectory)/LabelByInitialLetter.cpp$(PreprocessSuffix): LabelByInitialLetter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LabelByInitialLetter.cpp$(PreprocessSuffix) "LabelByInitialLetter.cpp"

$(IntermediateDirectory)/Main.cpp$(ObjectSuffix): Main.cpp $(IntermediateDirectory)/Main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label/Main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Main.cpp$(DependSuffix): Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Main.cpp$(DependSuffix) -MM "Main.cpp"

$(IntermediateDirectory)/Main.cpp$(PreprocessSuffix): Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Main.cpp$(PreprocessSuffix) "Main.cpp"

$(IntermediateDirectory)/ZonesAndWalls.cpp$(ObjectSuffix): ZonesAndWalls.cpp $(IntermediateDirectory)/ZonesAndWalls.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/Users/hieu/workspace/github/mosesdecoder.hieu/contrib/other-builds/manual-label/ZonesAndWalls.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ZonesAndWalls.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ZonesAndWalls.cpp$(DependSuffix): ZonesAndWalls.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ZonesAndWalls.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ZonesAndWalls.cpp$(DependSuffix) -MM "ZonesAndWalls.cpp"

$(IntermediateDirectory)/ZonesAndWalls.cpp$(PreprocessSuffix): ZonesAndWalls.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ZonesAndWalls.cpp$(PreprocessSuffix) "ZonesAndWalls.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


