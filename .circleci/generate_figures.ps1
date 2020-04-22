$pngOutDir = Resolve-Path (Join-Path $PSScriptRoot "..\docs\images\schema")
$generatorDir = Join-Path $PSScriptRoot "xsddiagram"
$generator = Join-Path $generatorDir "XSDDiagramConsole.exe"

function Clear-Gen() {
	# Get-ChildItem $pngOutDir\*
	Remove-Item $pngOutDir\*
}

function Get-XSDDiagram() {
	if (-Not (Test-Path $generator))
	{
		$uri = "https://github.com/clagms/xsddiagram/releases/download/XSDDiagram_1.2_FMI/XSDDiagram_1.2_FMI.zip"
		$zipFile = "XSDDiagram-Binary.zip"
		Invoke-WebRequest -Uri $uri -OutFile $zipFile
		Expand-Archive -Path $zipFile -DestinationPath $generatorDir -Force
		Remove-Item -Path $zipFile
	}
}

function Check-Existing() {
	Param(
		$file
	)
	If (Test-Path $file -PathType Leaf) {
		Write-Output "Error: File already exists."
		Exit 1
	}
}

function Export-Schema() {
	Param(
		$element,
		$expand,
		$schema = "fmi3ModelDescription.xsd",
		$outName = $element + ".png",
		[switch] $preview = $false
	)
	Write-Output "Generating schema for $element..."
	$out = Join-Path $pngOutDir $outName
	Check-Existing $out
	& $generator -o $out -r $element -e $expand -d -c -z 300 -a -no-gui -y (Resolve-Path (Join-Path $PSScriptRoot "..\schema\$schema"))
	if ($preview) {
		Start-Process $out
	}
}

Clear-Gen
Get-XSDDiagram

Export-Schema fmiModelDescription 1
Export-Schema UnitDefinitions 3
Export-Schema BaseUnit 2
Export-Schema DisplayUnit 2
Export-Schema TypeDefinitions 2
Export-Schema Float64 3 -schema fmi3Type.xsd
Export-Schema Int32 3 -schema fmi3Type.xsd
Export-Schema Boolean 3 -schema fmi3Type.xsd
Export-Schema Binary 3 -schema fmi3Type.xsd
Export-Schema Enumeration 3 -schema fmi3Type.xsd
Export-Schema Clock 3 -schema fmi3Type.xsd
Export-Schema LogCategories 3
Export-Schema DefaultExperiment 1
Export-Schema fmiTerminalsAndIcons 1 -schema fmi3TerminalsAndIcons.xsd
Export-Schema Terminals 2 -schema fmi3TerminalsAndIcons.xsd
Export-Schema TerminalMemberVariable 1 -schema fmi3TerminalsAndIcons.xsd
Export-Schema TerminalStreamMemberVariable 1 -schema fmi3TerminalsAndIcons.xsd
Export-Schema GraphicalRepresentation 2 -schema fmi3TerminalsAndIcons.xsd
Export-Schema VendorAnnotations 2
Export-Schema ModelVariables 2
Export-Schema fmi3VariableBase 1
Export-Schema Float64 1 -schema fmi3Variable.xsd -outName "Variable_Float64.png"
Export-Schema Int32 1 -schema fmi3Variable.xsd -outName "Variable_Int32.png"
Export-Schema Boolean 1 -schema fmi3Variable.xsd -outName "Variable_Boolean.png"
Export-Schema Binary 1 -schema fmi3Variable.xsd -outName "Variable_Binary.png"
Export-Schema Enumeration 1 -schema fmi3Variable.xsd -outName "Variable_Enumeration.png"
Export-Schema Annotations 2
Export-Schema ModelStructure 2
Export-Schema ModelExchange 2
Export-Schema BasicCoSimulation 2
