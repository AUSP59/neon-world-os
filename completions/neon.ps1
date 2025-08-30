# PowerShell completion (basic)
Register-ArgumentCompleter -CommandName neon -ScriptBlock {
  param($commandName, $parameterName, $wordToComplete, $commandAst, $fakeBoundParameters)
  $opts = @('--help','--version','--about-json','--help-json','--trace-out','--self-test-backtrace','doctor','metrics','trace')
  $opts | Where-Object { $_ -like "$wordToComplete*" } | ForEach-Object {
    [System.Management.Automation.CompletionResult]::new($_, $_, 'ParameterValue', $_)
  }
}
