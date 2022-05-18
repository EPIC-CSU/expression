/*****************************************************************\
*                                                                *
*  Copyright (C) Regents of University of California, 2003       *
*  This source code is a part of EXPRESSION project and is       *
*  copyrighted by the Regents of the University of California,   *
*  Irvine.                                                       *
*  The authors hereby grant permission to use this description   *
*  and its documentation for educational and non-commercial      *
*  purpose under the BSD license                                 *
*  (http://www.cecs.uci.edu/~express/BSD_License.txt). 	         *
*  The source code comes with no warranty and no author or       *
*  distributor accepts any responsibility for the consequences   *
*  of its use. Permission is granted to distribute this file in  *
*  compiled or executable form under the same conditions that    *
*  apply for source code. Permission is granted	to anyone to     *
*  make or distribute copies of this source code, either as      *
*  received or modified, in any medium, provided that all        *
*  copyright notices, permission and non warranty notices are    *
*  preserved, and that the distributor grants the recipient      *
*  permission for further redistribution as permitted by this    *
*  document. No written agreement, license, or royalty fee is    *
*  required for authorized use of this software.                 *
*                                                                *
*******************************************************************/
/* This copyright is included since we're using Cacti for some stuff */

/*------------------------------------------------------------
 *  Copyright 1994 Digital Equipment Corporation and Steve Wilton
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Digital a non-exclusive, unrestricted, royalty-
 * free right and license under any changes, enhancements or extensions
 * made to the core functions of the software, including but not limited to
 * those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to Digital any
 * such changes, enhancements or extensions that they make and inform Digital
 * of noteworthy uses of this software.  Correspondence should be provided
 * to Digital at:
 *
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Digital Equipment Corporation
 *                       100 Hamilton Avenue
 *                       Palo Alto, California  94301
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

#include <math.h>
#include <assert.h>
// #include <Cacti.h>

#define SensePowerfactor (Mhz)*(Vdd/2)*(Vdd/2)
#define Sense2Powerfactor (Mhz)*(2*.3+.1*Vdd)
#define Powerfactor (Mhz)*Vdd*Vdd
#define LowSwingPowerfactor (Mhz)*.2*.2

#define verbose debugSIMULATOR

typedef signed long counter_t;
typedef unsigned long quad_t;

/* set scale for crossover (vdd->gnd) currents */
double crossover_scaling = 1.2;
/* set non-ideal turnoff percentage */
double turnoff_factor = 0.1;

#define MSCALE (LSCALE * .624 / .2250)

/*----------------------------------------------------------------------*/

int pow2(int x)
 {
  return((int)pow(2.0,(double)x));
}

double logfour(double x)
{
  if (x<=0) fprintf(stderr,"%e\n",x);
  return( (double) (log(x)/log(4.0)) );
}


/* safer pop count to validate the fast algorithm */
/* int pop_count_slow(quad_t bits)
{
  int count = 0; 
  quad_t tmpbits = bits; 
  while (tmpbits) { 
    if (tmpbits & 1) ++count; 
    tmpbits >>= 1; 
  } 
  return count; 
} */

/* fast pop count */
/* int pop_count(quad_t bits)
{
#define T unsigned long long
#define ONES ((T)(-1)) 
#define TWO(k) ((T)1 << (k)) 
#define CYCL(k) (ONES/(1 + (TWO(TWO(k))))) 
#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k)) 
  quad_t x = bits; 
  x = (x & CYCL(0)) + ((x>>TWO(0)) & CYCL(0)); 
  x = (x & CYCL(1)) + ((x>>TWO(1)) & CYCL(1)); 
  BSUM(x,2); 
  BSUM(x,3); 
  BSUM(x,4); 
  BSUM(x,5); 
  return x; 
}
*/

int opcode_length = 8;
int inst_length = 32;

int nvreg_width;
int npreg_width;

/* compute bitline activity factors which we use to scale bitline power 
   Here it is very important whether we assume 0's or 1's are
   responsible for dissipating power in pre-charged stuctures. (since
   most of the bits are 0's, we assume the design is power-efficient
   enough to allow 0's to _not_ discharge 
*/
double compute_af(counter_t num_pop_count_cycle,counter_t total_pop_count_cycle,int pop_width) 
{
  double avg_pop_count;
  double af,af_b;

  if(num_pop_count_cycle)
    avg_pop_count = (double)total_pop_count_cycle / (double)num_pop_count_cycle;
  else
    avg_pop_count = 0;

  af = avg_pop_count / (double)pop_width;
  
  af_b = 1.0 - af;

  /*  printf("af == %f%%, af_b == %f%%, total_pop == %d, num_pop == %d\n",100*af,100*af_b,total_pop_count_cycle,num_pop_count_cycle); */

  return(af_b);
}

/* compute power statistics on each cycle, for each conditional clocking style.  Obviously
most of the speed penalty comes here, so if you don't want per-cycle power estimates
you could post-process 

See README.wattch for details on the various clock gating styles.

*/
void update_power_stats()
{


}




/* this routine takes the number of rows and cols of an array structure
   and attemps to make it make it more of a reasonable circuit structure
   by trying to make the number of rows and cols as close as possible.
   (scaling both by factors of 2 in opposite directions).  it returns
   a scale factor which is the amount that the rows should be divided
   by and the columns should be multiplied by.
*/
int squarify(int rows, int cols)
{
  int scale_factor = 1;

  if(rows == cols)
    return 1;

  /*
  printf("init rows == %d\n",rows);
  printf("init cols == %d\n",cols);
  */

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;

    /*
    printf("rows == %d\n",rows);
    printf("cols == %d\n",cols);
    printf("scale_factor == %d (2^ == %d)\n\n",scale_factor,(int)pow(2.0,(double)scale_factor));
    */

    if (rows/2 <= cols)
      return((int)pow(2.0,(double)scale_factor));
    scale_factor++;
  }

  return 1;
}

/* could improve squarify to work when rows < cols */

double squarify_new(int rows, int cols)
{
  double scale_factor = 0.0;

  if(rows==cols)
    return(pow(2.0,scale_factor));

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;
    if (rows <= cols)
      return(pow(2.0,scale_factor));
    scale_factor++;
  }

  while(cols > rows) {
    rows = rows*2;
    cols = cols/2;
    if (cols <= rows)
      return(pow(2.0,scale_factor));
    scale_factor--;
  }

  return 1;

}

/* void dump_power_stats(power)
     power_result_type *power;
{

}
*/

/*======================================================================*/



/* 
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

double driver_size(double driving_cap, double desiredrisetime) {
  double nsize, psize;
  double Rpdrive; 

  Rpdrive = desiredrisetime/(driving_cap*log(VSINV)*-1.0);
  psize = restowidth(Rpdrive,PCH);
  nsize = restowidth(Rpdrive,NCH);
  if (psize > Wworddrivemax) {
    psize = Wworddrivemax;
  }
  if (psize < 4.0 * LSCALE)
    psize = 4.0 * LSCALE;

  return (psize);

}

/* Decoder delay:  (see section 6.1 of tech report) */

double array_decoder_power(int rows, int cols, double predeclength, int rports, int wports, int cache)
{
  double Ctotal=0;
  double Ceq=0;
  int numstack;
  int decode_bits=0;
  int ports;
  double rowsb;

  /* read and write ports are the same here */
  ports = rports + wports;

  rowsb = (double)rows;

  /* number of input bits to be decoded */
  decode_bits=ceil((logtwo(rowsb)));

  /* First stage: driving the decoders */

  /* This is the capacitance for driving one bit (and its complement).
     -There are #rowsb 3->8 decoders contributing gatecap.
     - 2.0 factor from 2 identical sets of drivers in parallel
  */
  Ceq = 2.0*(draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1)) +
    gatecap(Wdec3to8n+Wdec3to8p,10.0)*rowsb;

  /* There are ports * #decode_bits total */
  Ctotal+=ports*decode_bits*Ceq;

  if(verbose)
    fprintf(stderr,"Decoder -- Driving decoders            == %g\n",.3*Ctotal*Powerfactor);

  /* second stage: driving a bunch of nor gates with a nand 
     numstack is the size of the nor gates -- ie. a 7-128 decoder has
     3-input NAND followed by 3-input NOR  */

  numstack = ceil((1.0/3.0)*logtwo(rows));

  if (numstack<=0) numstack = 1;
  if (numstack>5) numstack = 5;

  /* There are #rowsb NOR gates being driven*/
  Ceq = (3.0*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
	 gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0)))*rowsb;

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Decoder -- Driving nor w/ nand         == %g\n",.3*ports*Ceq*Powerfactor);

  /* Final stage: driving an inverter with the nor 
     (inverter preceding wordline driver) -- wordline driver is in the next section*/

  Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
	 numstack*draincap(WdecNORn,NCH,1)+
         draincap(WdecNORp,PCH,numstack));

  if(verbose)
    fprintf(stderr,"Decoder -- Driving inverter w/ nor     == %g\n",.3*ports*Ceq*Powerfactor);

  Ctotal+=ports*Ceq;

  /* assume Activity Factor == .3  */

  return(.3*Ctotal*Powerfactor);
}

double simple_array_decoder_power(int rows, int cols, int rports, int wports, int cache)
{
  double predeclength=0.0;
  return(array_decoder_power(rows,cols,predeclength,rports,wports,cache));
}


double array_wordline_power(int rows, int cols, double wordlinelength, int rports, int wports, int cache)
{
  double Ctotal=0;
  double Ceq=0;
  double Cline=0;
  double Cliner, Clinew=0;
  double desiredrisetime,psize,nsize;
  int ports;
  double colsb;

  ports = rports+wports;

  colsb = (double)cols;

  /* Calculate size of wordline drivers assuming rise time == Period / 8 
     - estimate cap on line 
     - compute min resistance to achieve this with RC 
     - compute width needed to achieve this resistance */

  desiredrisetime = Period/16;
  Cline = (gatecappass(Wmemcellr,1.0))*colsb + wordlinelength*CM3metal;
  psize = driver_size(Cline,desiredrisetime);
  
  /* how do we want to do p-n ratioing? -- here we just assume the same ratio 
     from an inverter pair  */
  nsize = psize * Wdecinvn/Wdecinvp; 
  
  if(verbose)
    fprintf(stderr,"Wordline Driver Sizes -- nsize == %f, psize == %f\n",nsize,psize);

  Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
    gatecap(nsize+psize,20.0);

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Wordline -- Inverter -> Driver         == %g\n",ports*Ceq*Powerfactor);

  /* Compute caps of read wordline and write wordlines 
     - wordline driver caps, given computed width from above
     - read wordlines have 1 nmos access tx, size ~4
     - write wordlines have 2 nmos access tx, size ~2
     - metal line cap
  */

  Cliner = (gatecappass(Wmemcellr,(BitWidth-2*Wmemcellr)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));
  Clinew = (2.0*gatecappass(Wmemcellw,(BitWidth-2*Wmemcellw)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));

  if(verbose) {
    fprintf(stderr,"Wordline -- Line                       == %g\n",1e12*Cline);
    fprintf(stderr,"Wordline -- Line -- access -- gatecap  == %g\n",1e12*colsb*2*gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0));
    fprintf(stderr,"Wordline -- Line -- driver -- draincap == %g\n",1e12*draincap(nsize,NCH,1) + draincap(psize,PCH,1));
    fprintf(stderr,"Wordline -- Line -- metal              == %g\n",1e12*wordlinelength*CM3metal);
  }
  Ctotal+=rports*Cliner+wports*Clinew;

  /* AF == 1 assuming a different wordline is charged each cycle, but only
     1 wordline (per port) is actually used */

  return(Ctotal*Powerfactor);
}

double simple_array_wordline_power(int rows, int cols, int rports, int wports, int cache)
{
  double wordlinelength;
  int ports = rports + wports;
  wordlinelength = cols *  (RegCellWidth + 2 * ports * BitlineSpacing);
  return(array_wordline_power(rows,cols,wordlinelength,rports,wports,cache));
}


double array_bitline_power(int rows, int cols, double bitlinelength, int rports,int wports, int cache)
{
  double Ctotal=0;
  double Ccolmux=0;
  double Cbitrowr=0;
  double Cbitroww=0;
  double Cprerow=0;
  double Cwritebitdrive=0;
  double Cpregate=0;
  double Cliner=0;
  double Clinew=0;
  int ports;
  double rowsb;
  double colsb;

  double desiredrisetime, Cline, psize, nsize;

  ports = rports + wports;

  rowsb = (double)rows;
  colsb = (double)cols;

  /* Draincaps of access tx's */

  Cbitrowr = draincap(Wmemcellr,NCH,1);
  Cbitroww = draincap(Wmemcellw,NCH,1);

  /* Cprerow -- precharge cap on the bitline
     -simple scheme to estimate size of pre-charge tx's in a similar fashion
      to wordline driver size estimation.
     -FIXME: it would be better to use precharge/keeper pairs, i've omitted this
      from this version because it couldn't autosize as easily.
  */

  desiredrisetime = Period/8;

  Cline = rowsb*Cbitrowr+CM2metal*bitlinelength;
  psize = driver_size(Cline,desiredrisetime);

  /* compensate for not having an nmos pre-charging */
  psize = psize + psize * Wdecinvn/Wdecinvp; 

  if(verbose)
    printf("Cprerow auto   == %g (psize == %g)\n",draincap(psize,PCH,1),psize);

  Cprerow = draincap(psize,PCH,1);

  /* Cpregate -- cap due to gatecap of precharge transistors -- tack this
     onto bitline cap, again this could have a keeper */
  Cpregate = 4.0*gatecap(psize,10.0);

  // TODO: this might be needed while estimating the clock power...
  // global_clockcap+=rports*cols*2.0*Cpregate;

  /* Cwritebitdrive -- write bitline drivers are used instead of the precharge
     stuff for write bitlines
     - 2 inverter drivers within each driver pair */

  Cline = rowsb*Cbitroww+CM2metal*bitlinelength;

  psize = driver_size(Cline,desiredrisetime);
  nsize = psize * Wdecinvn/Wdecinvp; 

  Cwritebitdrive = 2.0*(draincap(psize,PCH,1)+draincap(nsize,NCH,1));

  /* 
     reg files (cache==0) 
     => single ended bitlines (1 bitline/col)
     => AFs from pop_count
     caches (cache ==1)
     => double-ended bitlines (2 bitlines/col)
     => AFs = .5 (since one of the two bitlines is always charging/discharging)
  */

#ifdef STATIC_AF
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal+=(1.0-POPCOUNT_AF)*rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#else
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal += rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal += .3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#endif

  if(verbose) {
    fprintf(stderr,"Bitline -- Precharge                   == %g\n",1e12*Cpregate);
    fprintf(stderr,"Bitline -- Line                        == %g\n",1e12*(Cliner+Clinew));
    fprintf(stderr,"Bitline -- Line -- access draincap     == %g\n",1e12*rowsb*Cbitrowr);
    fprintf(stderr,"Bitline -- Line -- precharge draincap  == %g\n",1e12*Cprerow);
    fprintf(stderr,"Bitline -- Line -- metal               == %g\n",1e12*bitlinelength*CM2metal);
    fprintf(stderr,"Bitline -- Colmux                      == %g\n",1e12*Ccolmux);

    fprintf(stderr,"\n");
  }


  if(cache==0)
    return(Ctotal*Powerfactor);
  else
    return(Ctotal*SensePowerfactor*.4);
  
}


double simple_array_bitline_power(int rows, int cols, int rports, int wports, int cache)
{
  double bitlinelength;

  int ports = rports + wports;

  bitlinelength = rows * (RegCellHeight + ports * WordlineSpacing);

  return (array_bitline_power(rows,cols,bitlinelength,rports,wports,cache));

}

/* estimate senseamp power dissipation in cache structures (Zyuban's method) */
double senseamp_power(int cols)
{
  return((double)cols * Vdd/8 * .5e-3);
}

/* estimate comparator power consumption (this comparator is similar
   to the tag-match structure in a CAM */
double compare_cap(int compare_bits)
{
  double c1, c2;
  /* bottom part of comparator */
  c2 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
    draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);

  /* top part of comparator */
  c1 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2)+
		       draincap(Wcomppreequ,NCH,1)) +
    gatecap(WdecNORn,1.0)+
    gatecap(WdecNORp,3.0);

  return(c1 + c2);
}

/* power of depency check logic */
/*double dcl_compare_power(int compare_bits)
{
  double Ctotal;
  int num_comparators;
  
  num_comparators = (ruu_decode_width - 1) * (ruu_decode_width);

  Ctotal = num_comparators * compare_cap(compare_bits);

  return(Ctotal*Powerfactor*AF);
} */

double simple_array_power(int rows, int cols, int rports, int wports, int cache)
{
  if(cache==0)
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache));
  else
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache)+
	    senseamp_power(cols));
}


double cam_tagdrive(int rows, int cols, int rports, int wports)
{
  double Ctotal, Ctlcap, Cblcap, Cwlcap;
  double taglinelength;
  double wordlinelength;
  double nsize, psize;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  taglinelength = rows * 
    (CamCellHeight + ports * MatchlineSpacing);

  wordlinelength = cols * 
    (CamCellWidth + ports * TaglineSpacing);

  /* Compute tagline cap */
  Ctlcap = Cmetal * taglinelength + 
    rows * gatecappass(Wcomparen2,2.0) +
    draincap(Wcompdrivern,NCH,1)+draincap(Wcompdriverp,PCH,1);

  /* Compute bitline cap (for writing new tags) */
  Cblcap = Cmetal * taglinelength +
    rows * draincap(Wmemcellr,NCH,2);

  /* autosize wordline driver */
  psize = driver_size(Cmetal * wordlinelength + 2 * cols * gatecap(Wmemcellr,2.0),Period/8);
  nsize = psize * Wdecinvn/Wdecinvp; 

  /* Compute wordline cap (for writing new tags) */
  Cwlcap = Cmetal * wordlinelength + 
    draincap(nsize,NCH,1)+draincap(psize,PCH,1) +
    2 * cols * gatecap(Wmemcellr,2.0);
    
  Ctotal += (rports * cols * 2 * Ctlcap) + 
    (wports * ((cols * 2 * Cblcap) + (rows * Cwlcap)));

  return(Ctotal*Powerfactor*AF);
}

double cam_tagmatch(int rows, int cols,int rports,int wports)
{
  double Ctotal, Cmlcap;
  double matchlinelength;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  matchlinelength = cols * 
    (CamCellWidth + ports * TaglineSpacing);

  Cmlcap = 2 * cols * draincap(Wcomparen1,NCH,2) + 
    Cmetal * matchlinelength + draincap(Wmatchpchg,NCH,1) +
    gatecap(Wmatchinvn+Wmatchinvp,10.0) +
    gatecap(Wmatchnandn+Wmatchnandp,10.0);

  Ctotal += rports * rows * Cmlcap;

  // TODO: Need to include this while estimating the clock power.
//  global_clockcap += rports * rows * gatecap(Wmatchpchg,5.0);
  
// noring the nanded match lines 
  // TODO: check out why this is needed??
//  if(ruu_issue_width >= 8)
    Ctotal += 2 * gatecap(Wmatchnorn+Wmatchnorp,10.0);

  return(Ctotal*Powerfactor*AF);
}


double cam_array(int rows,int cols,int rports, int wports)
{
  return(cam_tagdrive(rows,cols,rports,wports) +
	 cam_tagmatch(rows,cols,rports,wports));
}


double selection_power(int win_entries)
{
  double Ctotal, Cor, Cpencode;
  int num_arbiter=1;

  Ctotal=0;

  while(win_entries > 4)
    {
      win_entries = (int)ceil((double)win_entries / 4.0);
      num_arbiter += win_entries;
    }

  Cor = 4 * draincap(WSelORn,NCH,1) + draincap(WSelORprequ,PCH,1);

  Cpencode = draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,1) + 
    2*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,2) + 
    3*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,3) + 
    4*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,4) + 
    4*gatecap(WSelEnn+WSelEnp,20.0) + 
    4*draincap(WSelEnn,NCH,1) + 4*draincap(WSelEnp,PCH,1);

// TODO: need to check on this...
//  Ctotal += ruu_issue_width * num_arbiter*(Cor+Cpencode);

  return(Ctotal*Powerfactor*AF);
}

/* very rough clock power estimates */
double total_clockpower(double die_length)
{
	return 0;
}

/* very rough global clock power estimates */
double global_clockpower(double die_length)
{
	return 0;
}


double compute_resultbus_power()
{
  return 0;
}

/* void calculate_power(power)
     power_result_type *power;
{

}
*/