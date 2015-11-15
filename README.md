## NAME

foamLookupEntry -- look up entry with a given key in a given dictionary

## SYNOPSIS

foamLookupEntry **-dict** [FILE] **-key** [KEY] **-batch**

## DESCRIPTION

The `foamLookupEntry` program searches a given dictionary file for an entry
with a given keyword. The idea for the utility was to replace `sed` in
`getApplication` function, because `sed` pattern is not portable.

The following options are available:

* **-dict**

  Specify dictionary file, if omitted data is read from stdin.

* **-key**

  Key to look up. Either just word, or dot-separated path through the
  sub-dictionaries.

* **-batch**
  
  Fail silently returning positive value.

## RETURN VALUES

* **1** - option **-key** is absent
* **2** - file specied by **-dict** option does not exist
* **4** - keyword specified by **-key** option is not found
* **8** - one of subdictionaries in dot-separated path was not found

## EXAMPLES

`getApplication` function could be reimplemented this way:

```sh
getApplication()
{
    foamLookupEntry -dict system/controlDict -key application -batch
    return $?
}
```

Access via dot-separated path to different values in fvSolution looks like:

```sh
$ foamLookupEntry -dict system/fvSolution -key solvers.U.solver
smoothSolver
$ foamLookupEntry -dict system/fvSolution -key solvers.p.preconditioner
DIC
$ foamLookupEntry -dict system/fvSolution -key PISO.nNonOrthogonalCorrectors
0
```

Reading dictionary from stdin:

```sh
$ cat system/controlDict | foamLookupEntry -key timeFormat
general
```
