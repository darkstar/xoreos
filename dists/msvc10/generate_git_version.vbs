Dim fso
Dim currentDirectory, gitDirectory, baseDirectory
Dim targetFile
Dim gitDirFound
Dim file
Dim gitHead, gitRev

Const ForReading = 1

Set fso = CreateObject("Scripting.FileSystemObject")

currentDirectory = fso.GetAbsolutePathName(".")
baseDirectory = currentDirectory
gitDirFound = fso.FolderExists(baseDirectory + "\.git")

targetFile = baseDirectory + "\build-aux\git-revision.h"

while (fso.GetParentFolderName(baseDirectory) <> "" and not gitDirFound) 
	baseDirectory = fso.GetParentFolderName(baseDirectory)
	gitDirFound = fso.FolderExists(baseDirectory + "\.git")
wend

if not gitDirFound then
	WScript.echo "No git checkout?"

	Set file = fso.CreateTextFile(targetFile, True)
	file.WriteLine("#define PACKAGE_STRING ""xoreos git build""")
	file.Close 

	WScript.Quit
end if

gitDirectory = baseDirectory + "\.git"

WScript.echo "Git found as " + gitDirectory

Set file = fso.OpenTextFile(gitDirectory + "\HEAD", ForReading)
gitHead = file.ReadLine
file.Close
gitHead = Trim(Replace(Right(gitHead, len(gitHead) - 4), "/", "\"))

WScript.echo "git head is " + gitHead

Set file = fso.OpenTextFile(gitDirectory + "\" + gitHead, ForReading)
gitRev = file.ReadLine
file.Close

gitRev = Left(gitRev, 8)

WScript.echo "git revision is r" + gitRev

Set file = fso.CreateTextFile(targetFile, True)
file.WriteLine("#define PACKAGE_STRING ""xoreos git " + gitRev + """")
file.Close
