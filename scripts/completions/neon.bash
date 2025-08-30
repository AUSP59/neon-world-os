# SPDX-License-Identifier: MIT

# SPDX-License-Identifier: MIT
_neon()
{
  COMPREPLY=( $(compgen -W "help ls cat echo mkdir touch ps kill meminfo mode" -- "${COMP_WORDS[1]}") )
}
complete -F _neon neon
