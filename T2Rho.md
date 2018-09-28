# T2Rho

This tool helps to directly compute densities from the output of [`V2T`](./V2T.md), using the equations given in [Goes et al. (2000)](https://doi.org/10.1029/1999JB900300).

## Compilation

Before compilation make sure to have Qt and qmake installed. Then go to this folder and run

```
qmake
make
```

This generates the `T2Rho` executable.

## Using T2Rho

Executing `T2Rho -h` displays the available console commands together with two examples.

```
usage: T2Rho File_In File_Out [options]

T2Rho is designed to directly process output files from V2T in
order to obtain densities from temperatures assuming a specific
mantle composition.

  Required input parameters:
  --------------------------
  File_In          Path and name of grid file containing x y z Vs
  File_Out         Output file name and path

  Option    Value Default Description
  ------    ----- ------- -----------
  -h                      This information
  -ERM      string  AK135 P calculation method AK135 or PREM
  -compc    vals          Custom rock composition
                          -compc Ol Opx Cpx Sp Gnt
  -compp    val         0 Use predefined rock compositions:
                          0 - Garnet Lherzolite after (Jordan,
                              1979; Goes et al, 2000)
                          1 - On-cratonic (Shapiro and Ritzwoller, 2004)
                          2 - Off-cratonic (Shapiro and Ritzwoller, 2004)
                          3 - Oceanic (Shapiro and Ritzwoller, 2004)
  -xfe      val       0.1 Define iron content of the rock in mole fraction
```

### Mandatory arguments

`T2Rho` requires the input file `File_In`, containing x y z and vs, and the name of the output file `File_Out`. Input units for z is masl, for vs km/s.

### Pressure calculation

Standard calculation of **pressure** uses the earth reference model AK135.
- options are `AK135`, `PREM` or `simple`
- `-ERM PREM` activates pressure calculation with PREM
- `-ERM simple` uses the average density defined with `-ra`
- an experimental feature is the pressure calculation using topography and crustal thickness. This is activated by using `-t_crust FILENAME` and `-z_topo FILENAME`, which both require EarthVision formatted grids containing crustal thickness and topographic elevation. The pressure is then calculated assuming constant density for the crust (`-rc 2890`) and mantle (`-rm 3300`)
- `-ra` defines an average density which is then used to calculate the pressure
