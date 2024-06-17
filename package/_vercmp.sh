# _vercmp g|l|e STRING0 STRING1
#
# Compare the version numbers contained in the two strings to determine their precedence
# according to the specifications defined in https://semver.org/spec/v2.0.0.html
# The build metadata will be ignored.
#
# options:
#
#   g:      compare if the version contained in STRING0 is greater than the version in STRING1
#   l:      compare if the version contained in STRING0 is less than the version in STRING1
#   e:      compare if the version contained in STRING0 is equal to the version in STRING1
#
# The return code is
#
#   0:      when the above comparison holds
#   1:      when the above comparison does not hold
#
[[ -z ${_BASHFUNC_VERCMP} ]] || return 0
_BASHFUNC_VERCMP=1

__bashfunc_vercmp_compare_ver() {
	local v0=${1} v1=${2} is_num=1 res= prefix= v0_tmp=0
	if [[ $v0 =~ ^([[:alpha:]]+)([[:digit:]]+)$ ]]; then
		prefix="${BASH_REMATCH[1]}"
		v0_tmp="${BASH_REMATCH[2]}"
		if [[ ${v1#$prefix} =~ ^([[:digit:]]*)$ ]]; then
			v0="$v0_tmp"
			v1="${BASH_REMATCH[1]}"
			: "${v1:=-1}"
		fi
	fi
	if [[ ! ${v0} =~ ^[0-9]+$ ]] || [[ ! ${v1} =~ ^[0-9]+$ ]]; then
		is_num=0
		: ${v0/-1/\\/}
		: ${v1/-1/\\/}
	fi
	if [[ ${is_num} == 1 ]]; then
		if (( ${v0} > ${v1} )); then
			res="g"
		elif (( ${v0} == ${v1} )); then
			res="e"
		elif (( ${v0} < ${v1} )); then
			res="l"
		fi
	else
		if [[ ${v0} > ${v1} ]]; then
			res="g"
		elif [[ ${v0} == ${v1} ]]; then
			res="e"
		elif [[ ${v0} < ${v1} ]]; then
			res="l"
		fi
	fi
	echo -n "${res}"
}
__bashfunc_vercmp_compare() {
	local v0 v1 vv0 vv1 len res
	OIFS=$IFS; IFS="."
	v0=(${1}) v1=(${2})
	IFS=$OIFS
	if [[ ${#v0[@]} -ge ${#v1[@]} ]]; then
		len=${#v0[@]}
	else
		len=${#v1[@]}
	fi
	for (( i=0; i<${len}; i++ )); do
		vv0=${v0[$i]:--1}
		vv1=${v1[$i]:--1}
		res=$(__bashfunc_vercmp_compare_ver ${vv0} ${vv1})
		if [[ ${res} != "e" ]]; then
			break
		fi
	done
	echo -n "${res}"
}
_vercmp() {
	local LC_ALL=C
	local action result
	case ${1} in
		g|l|e)
			action="${1}"
			;;
		*)
			echo "internal function error: _vercmp, unexpected argument '${1}'" >&2
			return 1
			;;
	esac
	shift

	local -a str0 str1
	local str0_pkgname str0_ver str0_pre
	local str1_pkgname str1_ver str1_pre

	OIFS=$IFS; IFS="-"
	str0=( $1 ) str1=( $2 )
	IFS=$OIFS

	for _str in "str0" "str1"; do
		local _strA="${_str}[@]" _strV="${_str}_ver" _strP="${_str}_pre" _strN="${_str}_pkgname"
		for _s in "${!_strA}"; do
			if [[ -n ${!_strV} ]]; then
				eval "${_str}_pre=\"${_s}\""
			elif [[ ${_s} =~ ^[vV]?[0-9]+\.? ]]; then
				_s="${_s#v}"
				eval "${_str}_ver=\"${_s#V}\""
			else
				if [[ -n ${!_strN} ]]; then
					eval "${_str}_pkgname+=\"-\""
				fi
				eval "${_str}_pkgname+=\"${_s}\""
			fi
		done
	done

	if [[ ${str0_pkgname} != ${str1_pkgname} ]]; then
		echo "internal function error: _vercmp, compare with different package names '${str0_pkgname}', '${str1_pkgname}'" >&2
		return 99
	fi

	result=$(__bashfunc_vercmp_compare ${str0_ver} ${str1_ver})
	if [[ ${result} == "e" ]]; then
		result=$(__bashfunc_vercmp_compare ${str0_pre:-zzzz} ${str1_pre:-zzzz})
	fi

	if [[ ${result} == ${action} ]]; then
		return 0
	else
		return 1
	fi
}

# vim:sw=8:ts=8:noexpandtab
