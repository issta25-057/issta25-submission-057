alias rm='rm -i'
alias cp='cp -i'
alias mv='mv -i'

alias ls='ls -hFG'
alias l='ls -lF'
alias ll='ls -alF'
alias lt='ls -ltrF'
alias ll='ls -alF'
alias lls='ls -alSrF'
alias llt='ls -altrF'

alias tarc='tar cvf'
alias tarcz='tar czvf'
alias tarx='tar xvf'
alias tarxz='tar xvzf'

alias g='git'
alias less='less -R'
alias os='lsb_release -a'
alias vi='vim'

# Colorize directory listing
alias ls="ls -ph --color=auto"

# Shell
export CLICOLOR="1"
if [ -f $HOME/.scripts/git-prompt.sh ]; then
  source $HOME/.scripts/git-prompt.sh
  export GIT_PS1_SHOWDIRTYSTATE="1"
  export PS1="\[\033[40m\]\[\033[34m\][ \u@\H:\[\033[36m\]\w\$(__git_ps1 \" \[\033[35m\]{\[\033[32m\]%s\[\033[35m\]}\")\[\033[34m\] ]$\[\033[0m\] "
else
  export PS1="\[\033[40m\]\[\033[34m\][ \u@\H:\[\033[36m\]\w\[\033[34m\] ]$\[\033[0m\] "
fi
export LS_COLORS="di=34:ln=35:so=32:pi=33:ex=1;40:bd=34;40:cd=34;40:su=0;40:sg=0;40:tw=0;40:ow=0;40:"

# Git
source $HOME/.scripts/git-completion.sh

source $HOME/.scripts/z.sh
