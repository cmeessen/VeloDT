# V2RhoT

C++ implementation for the conversion of vs and vp to density and temperature following the approach by [Goes et al. (2000)](http://onlinelibrary.wiley.com/doi/10.1029/1999JB900300/abstract). A python implementation with slightly different functionality is available [here](https://github.com/cmeessen/VelocityConversion).

## Compilation

Before compilation make sure to have Qt and qmake installed. Then go to this folder and run

```
qmake
make
```

`qmake` generates the makefile and `make` compiles the `V2RhoT` binary.

## Using V2RhoT

Executing `V2RhoT -h` displays the available console commands together with two examples.

```
usage: V2RhoT File_In File_Out -type <P,S> [options]

  Required input parameters:
  --------------------------
  File_In          Path and name of grid file containing x y z Vs
  File_Out         Output file name and path
  type             Specify S or P for input wave type

  Units
  -----
  depth     m      Values below MSL must be negative
  velocity  m/s

  Option    Value Default Description
  ------    ----- ------- -----------
  -AlphaT                 Activates T-dependency of alpha. Default
                          is constant alpha.
  -compc    vals          Custom rock composition, see example 2
                          -comp Ol Opx Cpx Sp Gnt
  -compp    val         0 Use predefined rock compositions:
                          0 - Garnet Lherzolite after (Jordan,
                              1979; Goes et al, 2000)
                          1 - On-cratonic (Shapiro and Ritzwoller, 2004)
                          2 - Off-cratonic (Shapiro and Ritzwoller, 2004)
                          3 - Oceanic (Shapiro and Ritzwoller, 2004)
  -ERM      string  AK135 P calculation method AK135, PREM or simple
  -f        val    1/0.02 Define custom wave frequency in Hz.
  -fdamp    val     0.025 Iteration dampening
  -minDB    1 or 2      1 Mineral property database by
                          1 - Cammarano et al. (2003)
                          2 - Goes et al. (2000)
  -petrel                 Output is written as Petrel Points with  Attribtues
  -prop                   Print mineral properties
  -Q        1 or 2      1 Anelasticity paramters Q
                          1 - Sobolev et al. (1996)
                          2 - Berckhemer et al. (1982)
  -rc       val      2890 Crustal density in kg/m3
  -rm       val      3300 Mantle density in kg/m3
  -ra       val      3100 Average density in kg/m3 used in '-ERM simple'
  -scaleZ   val         1 Scale every z-value in File_In by this value
  -scaleV   val         1 Scale every Vs-value in File_In by this value
  -scatter                Use scattered data as input instead of regular grid
  -t        val       0.1 Threshold in K where Temperature iteration stops
  -Tstart   val    273.15 Iteration starting temperature
  -t_crust  path          EarthVision file for crustal thickness
  -writedRdT              Writes used dRho/dT tables for minerals to a text file
  -xfe      val         0 Define iron content of the rock in mole fraction
  -z_topo   path          EarthVision file for topographic elevation

Example 1:
V2RhoT Vs.dat T.dat -type P -rc 2870 -rm 3300 -t_crust crust.dat -z_topo  topo.dat

Example 2:
V2RhoT Vs.dat T.dat -type S -compc 0.82 0.144 0.0 0.0 0.036
```

### Mandatory arguments

**Mandatory** arguments are `File_In`, `File_Out` and `-type <P,S>`. The conversion requires **SI units** in the input files for depth (masl) and velocity (m/s).

Depending on wave type, the frequency will be changed to
- vp -> f = 1 Hz
- vs -> f = 0.02 Hz
- can be changed with `-f` flag

**Important:** the temperature-dependent computation of the *expansion coefficient* in `V2RhoT` is, in contrast to the method described in Goes et al. (2000), deactivated. Instead, a *constant* expansion coefficient is assumed. The reason for this decision is that pressure and temperature act in opposite directions on the value of alpha. Therefore, it is assumed that both influences cancel out each other. Temperature-dependent calculation can be activated with `-AlphaT` flag.


### Rock composition

By default, the **rock composition** is assumed to be a Garnet Lherzolite (Jorand, 1979)
- `-compp` allos to use one of four predefined rock compsitions
- `-compc ol opx cpx sp gnt` assigns a custom rock composition given the fractions of minerals in the rock
- **iron content** is by default 0. To obtain realistic results it must be specified with `-xfe`

### Pressure calculation

Standard calculation of **pressure** uses the earth reference model AK135.
- options are `AK135`, `PREM` or `simple`
- `-ERM PREM` activates pressure calculation with PREM
- `-ERM simple` uses the average density defined with `-ra`
- an experimental feature is the pressure calculation using topography and crustal thickness. This is activated by using `-t_crust FILENAME` and `-z_topo FILENAME`, which both require EarthVision formatted grids containing crustal thickness and topographic elevation. The pressure is then calculated assuming constant density for the crust (`-rc 2890`) and mantle (`-rm 3300`)
- `-ra` defines an average density which is then used to calculate the pressure
