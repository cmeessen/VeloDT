V2RhoT = ../bin/V2RhoT
V2T = ../bin/V2T
# Plot properties
FIGURE = results.ps
ROI_T = 450/1450/-200/-50
ROI_R = 3290/3390/-200/-50
MAPHEIGHT = 10
MAPWIDTH = 6

# Targets

clean:
	rm Vs.dat Vp.dat V2RhoT_Vs.dat V2RhoT_Vp.dat V2T_Vs.dat gmt.*

plot: convert
	# Plot T-Depth
	gmt gmtset PS_MEDIA A0 MAP_ORIGIN_Y 10
	# GGV T Vs
	gmt psxy V2RhoT_Vs.dat -i4,2+s0.001 -JX$(MAPWIDTH)/$(MAPHEIGHT) -R$(ROI_T) -W1p,red -BWSne+t'Temperature' -Bxaf+l'T / \260C' -Byaf+l'Depth / km' -P -K > $(FIGURE)
	# GGV T Vp
	gmt psxy V2RhoT_Vp.dat -i4,2+s0.001 -J -R -W1p,blue -O -P -K >> $(FIGURE)
	# PMK T Vs
	gmt psxy V2T_Vs.dat -i3,2+s0.001 -J -R -W1p,red,. -O -P -K >> $(FIGURE)
	# Legend
	echo 'N 1' > gmt.legend
	echo 'H 12p Helvetica Converted from' >> gmt.legend
	echo 'S 0.25 - 10p - 2p,red  0.5 Vs (Goes et al., 2000)' >> gmt.legend
	echo 'S 0.25 - 10p - 2p,blue 0.5 Vp (Goes et al., 2000)' >> gmt.legend
	echo 'S 0.25 c 2p red - 0.5 Vs (Priestley and McKenzie, 2006)' >> gmt.legend
	gmt pslegend gmt.legend -DJBR+w8+o0.5/2+jTC -F -R -J -O -P -K >> $(FIGURE)
	# GGV Rho Vs
	gmt psxy V2RhoT_Vs.dat -i5,2+s0.001 -J -R$(ROI_R) -W1p,red -X7 -BwSne+t'Density' -Bx+l'@~r@~ / kg/m@+3@+' -Baf -O -P -K >> $(FIGURE)
	# GGV Rho Vp
	gmt psxy V2RhoT_Vp.dat -i5,2+s0.001 -J -R -W1p,blue -O -P >> $(FIGURE)
	gmt psconvert -A -Tg -Z $(FIGURE) -E100 -Qt -Qg

convert: createfiles Vs.dat Vp.dat
	$(V2RhoT) Vs.dat V2RhoT_Vs.dat -type S -ERM PREM -scaleZ -1000 -scaleV 1000 -compp 0 -xfe 0.086
	$(V2RhoT) Vp.dat V2RhoT_Vp.dat -type P -ERM PREM -scaleZ -1000 -scaleV 1000 -compp 0 -xfe 0.086
	$(V2T) Vs.dat V2T_Vs.dat -ERM PREM -scaleZ -1000

createfiles:
	awk '{if(NR>3){print($$1,$$2,$$3,$$4)}}' PREM.dat > Vp.dat
	awk '{if(NR>3){print($$1,$$2,$$3,$$5)}}' PREM.dat > Vs.dat
