#!/usr/bin/perl5

print "Translating:\n";

%opcodemap = (
# format:
#             "mips opcode" => ("EXPRESS opcode", _print_warning_),

	      "la"        => "ICONSTANT=",
	      "lb"        => "QIVLOAD",
	      "lbu"       => "QIVLOADU",
	      "lh"        => "HIVLOAD",
	      "lhu"       => "HIVLOADU",
	      "lw"        => "IVLOAD",
	      "ld"        => "DVLOAD",
	      "ulh"       => "HIVLOAD",
	      "ulhu"      => "HIVLOADU",
	      "ulw"       => "IVLOAD",
	      "li"        => "ICONSTANT",
	      "lui"       => "ICONSTANT",   #not any more   "16",          #left shift the constant 16 bits before

              # stores: first argument should go last (source)
	      "sb"        => "QIVSTORE>",
	      "sd"        => "DVSTORE>",
	      "sh"        => "HIVSTORE>",
	      "sw"        => "IVSTORE>",
	      "ush"       => "HIVSTORE>",
	      "usw"       => "IVSTORE>",

	      "add"       => "IADD",
	      "addu"      => "IADD",
	      "addiu"     => "IADD",
	      "and"       => "IAND",
	      "andi"      => "IAND",

# --removed--	      # next instructions: duplicate source1 as destination
	      "div"       => "IDIV",
	      "divu"      => "IDIVU",
	      "xor"       => "IXOR",
	      "xori"      => "IXOR",
	      "mul"       => "IMUL",
	      "mulo"      => "IMUL",
	      "mulou"     => "IMULU",
	      "nor"       => "INOR",
	      "nori"      => "INOR",
	      "or"        => "IOR",
	      "ori"       => "IOR",
	      "seq"       => "IEQ",
	      "sgt"       => "IGT",
	      "sge"       => "IGE",
	      "sgeu"      => "IGEU",
	      "sgtu"      => "IGTU",
	      "slt"       => "ILT",
	      "sle"       => "ILE",
	      "sleu"      => "ILEU",
	      "sltu"      => "ILTU",
	      "sne"       => "INE",
	      "sub"       => "ISUB",
	      "subu"      => "ISUB",
	      "rem"       => "IREM",
	      "remu"      => "IREMU",
	      "sra"       => "IRASH",  
	      "sll"       => "ILLSH",
	      "srl"       => "IRLSH",  

	      "abs"       => "IABS",

# --removed--	      # next instructions: duplicate source1 as destination
	      "neg"       => "INEG",
	      "negu"      => "INEG",
	      "not"       => "IXOR>1",                # INOR0

	      "move"      => "IASSIGN",

	      "mult"      => "IMUL<hilo",           # destination = $hilo
	      "multu"     => "IMULU<hilo",          # destination = $hilo

	      "j"         => "GOTO",
	      "jal"       => "CALL",
	      "beq"       => "IEQ+IF",
	      "bgt"       => "IGT+IF",
	      "bge"       => "IGE+IF",
	      "bgeu"      => "IGEU+IF",
	      "bgtu"      => "IGTU+IF",
	      "blt"       => "ILT+IF",
	      "ble"       => "ILE+IF",
	      "bleu"      => "ILEU+IF",
	      "bltu"      => "ILTU+IF",
	      "bne"       => "INE+IF",

	      "b"         => "GOTO",
	      "bal"       => "CALL",
	      "beql"      => "IEQ+IF",
	      "bgtl"      => "IGT+IF",
	      "bgel"      => "IGE+IF",
	      "bgeul"     => "IGEU+IF",
	      "bgtul"     => "IGTU+IF",
	      "bltl"      => "ILT+IF",
	      "blel"      => "ILE+IF",
	      "bleul"     => "ILEU+IF",
	      "bltul"     => "ILTU+IF",
	      "bnel"      => "INE+IF",

	      "beqz"      => "IEQ+IF>0",
	      "bgez"      => "IGE+IF>0",
	      "bgtz"      => "IGT+IF>0",
	      "blez"      => "ILE+IF>0",
	      "bltz"      => "ILT+IF>0",
	      "bnez"      => "INE+IF>0",

	      "beqzl"     => "IEQ+IF>0",
	      "bgezl"     => "IGE+IF>0",
	      "bgtzl"     => "IGT+IF>0",
	      "blezl"     => "ILE+IF>0",
	      "bltzl"     => "ILT+IF>0",
	      "bnezl"     => "INE+IF>0",

	      "mfhi"      => "MFHI>hilo",
	      "mflo"      => "MFLO>hilo",
	      "mthi"      => "MTHI<hilo",
	      "mtlo"      => "MFLO<hilo",

	      "syscall"   => "SYSCALL",

	      "lwc1"      => "IVLOAD",
	      "ldc1"      => "DVLOAD",

	      # stores: first argument should go last (source)
	      "swc1"      => "IVSTORE>",
	      "sdc1"      => "DVSTORE>",

	      "mfc1"      => "MFC1",
	      "mtc1"      => "MTC1>",
	      "dmfc1"     => "DMFC1",
	      "dmtc1"     => "DMTC1>",

	      "bc1f"      => "IXOR>1+IFFT>cc0",
	      "bc1t"      => "IFFT>cc0",
	      "bc1fl"     => "IXOR>1+IFFT>cc0",
	      "bc1tl"     => "IFFT>cc0",

	      "cfc1"      => "IASSIGN>cc0",
	      "ctc1"      => "IASSIGN>cc0",

	      "l.d"       => "DVLOAD",
	      "l.s"       => "FVLOAD",

	      "ldxc1"     => "DVLOAD",
	      "lwxc1"     => "FVLOAD",

	      "li.d"      => "DCONSTANT",
	      "li.s"      => "FCONSTANT",

	      # stores: first argument should go last (source)
	      "s.d"       => "DVSTORE>",
	      "s.s"       => "FVSTORE>",

	      "sdxc1"     => "DVSTORE>",
	      "swxc1"     => "FVSTORE>",

	      "abs.d"     => "DABS",
	      "abs.s"     => "FABS",

	      "neg.d"     => "DNEG",
	      "neg.s"     => "FNEG",

	      "add.d"     => "DADD",
	      "add.s"     => "FADD",

	      "div.d"     => "DDIV",
	      "div.s"     => "FDIV",

	      "mul.d"     => "DMUL",
	      "mul.s"     => "FMUL",

	      "sub.d"     => "DSUB",
	      "sub.s"     => "FSUB",

	      "cvt.s.d"   => "CVTSD",
	      "cvt.s.w"   => "CVTSI",
	      "cvt.d.s"   => "CVTDS",
	      "cvt.d.w"   => "CVTDI",
	      "cvt.w.s"   => "CVTIS",
	      "cvt.w.d"   => "CVTID",

	      "trunc.w.s" => "TRUNCIS-",
	      "trunc.w.d" => "TRUNCID-",
	      "round.w.s" => "ROUNDIS-",
	      "round.w.d" => "ROUNDID-",
	      "ceil.w.d"  => "CEILID-",
	      "ceil.w.s"  => "CEILIS-",
	      "ceilu.w.d" => "CEILUID-",
	      "ceilu.w.s" => "CEILUIS-",
	      "floor.w.d" => "FLOORID-",
	      "floor.w.s" => "FLOORIS-",
	      "flooru.w.d"=> "FLOORUID-",
	      "flooru.w.s"=> "FLOORUIS-",
	      "roundu.w.d"=> "ROUNDID-",
	      "roundu.w.s"=> "ROUNDIS-",
	      "truncu.w.s"=> "TRUNCUIS-",
	      "truncu.w.d"=> "TRUNCUID-",

	      "cvt.s.l"   => "CVTSI",
	      "cvt.d.l"   => "CVTDI",
	      "cvt.l.s"   => "CVTIS",
	      "cvt.l.d"   => "CVTID",

	      "trunc.l.s" => "TRUNCIS",
	      "trunc.l.d" => "TRUNCID",
	      "round.l.s" => "ROUNDIS",
	      "round.l.d" => "ROUNDID",
	      "ceil.l.s"  => "CEILIS",
	      "ceil.l.d"  => "CEILID",
	      "floor.l.s" => "FLOORIS",
	      "floor.l.d" => "FLOORID",

	      "c.f.d"	  => "ICONSTANTcc00",            # ($cc0 0)
	      "c.f.s"	  => "ICONSTANTcc00",            # ($cc0 0)

	      "c.un.d"	  => "ICONSTANTcc00",            # ($cc0 0)
	      "c.un.s"	  => "ICONSTANTcc00",            # ($cc0 0)

	      "c.eq.d"	  => "DEQcc0",
	      "c.eq.s"	  => "FEQcc0",

	      "c.ueq.d"	  => "DEQcc0",
	      "c.ueq.s"	  => "FEQcc0",

	      "c.olt.d"	  => "DLTcc0",
	      "c.olt.s"	  => "FLTcc0",

	      "c.ult.d"	  => "DLTcc0",
	      "c.ult.s"	  => "FLTcc0",

	      "c.ole.d"	  => "DLEcc0",
	      "c.ole.s"	  => "FLEcc0",

	      "c.ule.d"	  => "DLEcc0",
	      "c.ule.s"	  => "FLEcc0",

	      "c.sf.d"	  => "ICONSTANTcc00",            # ($cc0 0)
	      "c.sf.s"	  => "ICONSTANTcc00",            # ($cc0 0)

	      "c.ngle.d"  => "ICONSTANTcc00",            # ($cc0 0)
	      "c.ngle.s"  => "ICONSTANTcc00",            # ($cc0 0)

	      "c.seq.d"	  => "DEQcc0",
	      "c.seq.s"	  => "FEQcc0",

	      "c.ngl.d"	  => "DEQcc0",
	      "c.ngl.s"	  => "FEQcc0",

	      "c.lt.d"	  => "DLTcc0",
	      "c.lt.s"	  => "FLTcc0",

	      "c.nge.d"	  => "DLTcc0",
	      "c.nge.s"	  => "FLTcc0",

	      "c.le.d"	  => "DLEcc0",
	      "c.le.s"	  => "FLEcc0",

	      "c.ngt.d"	  => "DLEcc0",
	      "c.ngt.s"	  => "FLEcc0",

	      "mov.s"	  => "FASSIGN",
	      "mov.d"	  => "DASSIGN",

	      "break"     => "NOP~",
	      "nop"       => "NOP~",
);

@warning_opcodes = ();

$ARGV[0] =~ /^(.*)\.s$/;
$FILE_BASE = $1;

open (IN_FILE, "$ARGV[0]") || die "Cannot open input file ${ARGV[0]}";
open (OUT_FILE, ">${FILE_BASE}.x") || die "Cannot open output file ${FILE_BASE}.x";

$source = join('', <IN_FILE>);

# hack for processing the last line too
$source .= " \n";

# remove all register saving instructions from beginning of file
# i.e. between "main:" and "jal	__gccmain"
# $source =~ /(^.*main:\s*)(\n.*)(\s*jal\s+__gccmain.*)$/s;
# $before = $1;
# $saves = $2;
# $after = $3;
# $saves =~ s/\n/\n\t\t\t\t\t\t;/sg;
# $source = $before.$saves.$after;


# remove all register restoring instructions from end of file
# i.e. between "move $sp, $fp" and "j $31"
# $source =~ /(^.*)(\n\s*move\s+\$sp,\s*\$fp\s*\n.*)(\s*j\s+\$31.*)$/s;
# $before = $1;
# $saves = $2;
# $after = $3;
# $saves =~ s/\n/\n\t\t\t\t\t\t;/sg;
# $source = $before.$saves.$after;



# find all declarations
@lines = split(/\n/, $source);
$source = "";
$data = 0;
$global = " (IS_GLOBAL 0)";
$good = 0;   # 1 if at least a .globl or label

$line = shift(@lines);
while ($#lines >= 0) {

    if ($line =~ /^\s*\.[sr]??data/) {
	$data = 1;
	$source .= "\t\t\t\t\t\t;$line\n";

	$line = shift(@lines);
	next;
    }

    if ($line =~ /^\s*\.text/) {
	$data = 0;
	$source .= "\t\t\t\t\t\t;$line\n";

	# added 03/19/02
	$global = " (IS_GLOBAL 0)";
	$good = 0;

	$line = shift(@lines);
	next;
    }

    if ($line =~ /^\s*\.globl\s+\S+\s*$/) {
	$global = " (IS_GLOBAL 1)";
	$source .= "\t\t\t\t\t\t;$line\n";
	$good = 1;

	$line = shift(@lines);
	next;
    }

    if ($data == 1) {  # we are in the data section

	$comments = "";
	$unmod = "";
	$name = "";
	$align = "";
	$space = "";
	$word = "";
	$half = "";
	$byte = "";
	$ascii = "";

	if ($line =~ /^\s*\.s?data$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";

	    $line = shift(@lines);
	}

	if ($line =~ /^\s*\.globl\s+\S+\s*$/) {
	    $global = " (IS_GLOBAL 1)";
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";
	    $good = 1;
	    
	    $line = shift(@lines);
	}

	if ($line =~ /^\s*\.sdata\s*$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";

	    $line = shift(@lines);
	}

	if ($line =~ /^\s*\.data$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";

	    $line = shift(@lines);
	}

	if ($line =~ /^\s*\.stabs\s+\"([^:]+):[^\"]+\".*$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";
	    #$name = $1;

	    $line = shift(@lines);
	}

	if ($line =~ /^\s*\.align\s+(\d+)\s*$/) {
	    $align .= " (ALIGN $1)";
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";

	    $line = shift(@lines);
	}
	
	if ($line =~ /^\s*\.type\s+.*$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";

	    $line = shift(@lines);
	}

	if ($line =~ /^\s*\.size\s+.*$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";

	    $line = shift(@lines);
	}

	if ($line =~ /^\s*([^:]+):$/) {
	    $comments .= "\t\t\t\t\t\t;$line\n";
	    $unmod .= "$line\n";
	    $good = 1;
	    $name = $1;

	    $line = shift(@lines);
	    
	    while (1) {
		if ($line =~ /^\s*\.space\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (SPACE ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.word\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (WORD ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.half\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (HALF ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.byte\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (BYTE ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.double\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (DOUBLE ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.dword\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (DWORD ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.float\s+(\S+)$/) {
		    $value = $1;
		    $value =~ s/^([a-zA-Z])/_$1/;
		    $value =~ s/^\$//;
		    $word .= " (FLOAT ${value})";
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.ascii\s+\"(.*)\"$/) {
		    $ascii .= $1;
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} elsif ($line =~ /^\s*\.asciiz\s+(\".*\")$/) {
		    $ascii .= $1;
		    $comments .= "\t\t\t\t\t\t;$line\n";
		    $unmod .= "$line\n";

		    $line = shift(@lines);
		} else {
		    last;
		}
	    }

	    if ($good == 1) {
		if (!($name =~ /^\$/)) {
		    $name = "_" . $name;
		} else {
		    $name =~ s/^\$//;
		}
		if ($ascii ne "") {
		    $ascii = " (ASCII \"${ascii}\")"
		}
		$source .= "(DECL${global}${align} ${name}${space}${word}${half}${byte}${ascii})\n$comments";

		$global = " (IS_GLOBAL 0)";
		$good = 0;
	    } else {
		$source .= $unmod;
	    }

	} else {
	    $source .= $unmod;

	    $source .= "$line\n";

	    $line = shift(@lines);
	}

	next;
    }
    
    $source .= "$line\n";

    $line = shift(@lines);
}

# find all global DECLs
print "\tglobal declarations... ";
$source =~ s/^(\.stabs\s+\"(\w+):[^\"]+\",\d+,\d+,\d+,\w+\n)\n(\s*\.comm\s+\2,(\d+))$/(DECL (IS_GLOBAL 1) (ALIGN 2) _$2 (SPACE $4))\t\t;$1\t\t\t\t\t\t\t\t;$3/mg;
print "done\n";

# find all local DECLs
print "\tlocal declarations... ";
$source =~ s/^(\s*\.globl.*\n\s*\.lcomm\s+(\S+),(\d+))$/(DECL (IS_GLOBAL 1) (ALIGN 2) _$2 (SPACE $3))\t\t;$1/mg;

$source =~ s/^(\s*\.lcomm\s+(\S+),(\d+))$/(DECL (IS_GLOBAL 0) (ALIGN 2) _$2 (SPACE $3))\t\t;$1\t\t\t\t\t\t\t\t;$3/mg;
print "done\n";

# find all ASCII decls
print "\traw data definitions... ";
@lines = split(/\n/, $source);
$source = "";
while ($#lines >= 0) {
    $line = shift(@lines);
    if ($line =~ /^(\s*\.[rs]data)$/) {
        $source .= "\t\t\t\t\t\t;$1\n";
        $line = shift(@lines);
        $old_lines = "";
        while ($line =~ /^(\s*\.align\s+(\d+))$/) {
	    $old_lines .= "$1\n";
	    $new_lines = "(DECL\t(IS_GLOBAL 0)\n\t(ALIGN $2)\t\t\t\t;$1\n";
	    $line = shift(@lines);
	    if ($line =~ /^(\$(\w+):)/) {
		$old_lines .= "$1\n";
		$new_lines .= "\t$2\t\t\t\t\t;$1\n";
		$line = shift(@lines);
		if ($line =~ /^(\s*\.ascii\s+\"(.*)\")$/) {
		    $new_lines .= "\t(ASCII \"";
		    $old_lines = "";
		    do {
			$old_lines .= "\t\t\t\t\t\t;$1\n";
			$new_lines .= $2;
			$line = shift(@lines);
		    } while ($line =~ /^(\s*\.ascii\s+\"(.*)\")$/);
		    $new_lines .= "\")\n";
		    $new_lines .= $old_lines;
		    $new_lines .= ")\n";
		    $source .= $new_lines;
		} else {
		    $source .= $old_lines;
		}
	    } else {
		$source .= $old_lines;
	    }
        }
        unshift @lines, $line;
    } else {
        $source .= "$line\n";
    }
}

print "done\n";

# find all LOCATIONs
print "\tlocations... ";
$source =~ s/^((\$L\w*):\n)(\s*.stabn \d+,\d+,(\d+),\2)$/(LOCATION $4)\t\t\t\t\t;$1\t\t\t\t\t\t;$3/mg;
print "done\n";

# find all LABELs
print "\tlabels... ";
$source =~ s/^((\w+):)$/(LABEL _$2)\t\t\t\t\t;$1/mg;
$source =~ s/^(\$(L\d+):)$/(LABEL $2)\t\t\t\t\t;$1/mg;
$source =~ s/^((\S+):)$/\t\t\t\t\t\t;$1/mg;
print "done\n";

# find all GLOBALs
print "\tglobals... ";
$source =~ s/^(\s*\.globl\s+(\w+))$/\t(GLOBAL _$2)\t\t;$1/mg;
print "done\n";

# find proc boundaries
print "\tproc boundaries...  ";
$source =~ s/^(\s*\.ent\s+(\w+))$/\t(PROC_BEGIN\t_$2\t\t\t;$1/mg;
$source =~ s/^(\s*\.end\s+(\w+))$/)\t ; END OF _$2\t;$1\n/mg;
print "done\n";

# find all proc parms
print "\tparms... ";
# find all return values
$source =~ s/^(\s*jal\s+\S+).*\n\s*\#APP.*\n\s*\#\s*\(retvalue\s+([^\)]+)\).*\n\s*\#NO_APP.*$/$1,$2\t\t\# return value hack/mg;
$source =~ s/^(\s*\#APP)$/(PSEUDOS\t\t\t\t\t;$1/mg;
$source =~ s/^(\s*\#NO_APP)$/)\t\t\t\t\t\t;$1/mg;
$source =~ s/^(\s*\#\s*\(parm\s+([^\)]+)\))$/\t(parm $2)\t\t\t\t;$1/mg;
print "done\n";

# not any more
# replace 'lui' \n 'addiu' (load high, low) by 'la'
#$source =~ s/^\s*lui\s+([^)]+),\s*%hi\(([^)]+)\).*\n\s*addiu\s+(\1),\s*(\1),\s*%lo\(\2\).*$/\tla\t$1,$2/mg;

# replace 
$source =~ s/%hi\(([^\)]+)\)/gcchi_$1/mg;
$source =~ s/%lo\(([^\)]+)\)/gcclo_$1/mg;

# translate the instructions
print "\tinstructions... ";
$tmp_source = "";
foreach $line (split(/\n/, $source)) {
    if ($line =~ /^\s*([a-z]\S*)(.*)$/) {
        $opc = $1;
        $params = $2;
        if (exists $opcodemap{$opc}) {
	    $opcode = $opcodemap{$opc};
	    if (grep(/^$opc$/, @warning_opcodes)) {
		print "Warning: $opc\n";
	    }
	    $params =~ s/^\s+//;
	    $params =~ s/\s*\#.*$//;
	    $params =~ tr /,()/  /d;
	    $params =~ s/[ \t]+([a-zA-Z_%]\w*)/ _$1/g;
	    $params =~ s/^\s*(\S)/\t$1/;
	    $tmp_source .= "\t($opcode$params)\t\t\t;$line\n";
	} else {
	    die "Error: Opcode not found: \"$opc\"\n";
	}
    } else {
        $tmp_source .= "$line\n";
    }
}
$source = $tmp_source;
$tmp_source = "";
print "done\n";

# translate special instructions
print "\tspecial instructions... ";
# not
#$source =~ s/^\s*\(INOR0\s+(\S+)\)/\t(INOR\t$1 \$0)/mg;
# not any more (replaced by IRLSH, IRASH)
# right shifts (suffix '-1')
#$source =~ s/^\s*\(([A-Z]+)\-1\s+(\S+)\s+(\S+)\s+(\S+)\)/\t($1\t$2 $3 -$4)/mg;
# calls
$source =~ s/^(\s*\(CALL\s+)([^)^\$][^)]*)\)/$1_$2)/mg;
# not any more 
# lui - left shift the constant 16 bits before (suffix '16')
#$source =~ s/^\s*\(ICONSTANT16\s+(\S+)\s+(\S+)\)/"\t(ICONSTANT\t$1 " . eval($2 << 16) . ")"/mge;
# stores: first argument should go last (source) (suffix '>')
$source =~ s/^\s*\(([A-Z1]+)>\s+(\S+)\s+([^)]+)\)/\t($1\t$3 $2)/mg;
## duplicate source1 as destination for some instructions (suffix '<')
#$source =~ s/^\s*\(([A-Z]+)<\s+(\S+)\s+([^)]+)\)/\t($1\t$2 $2 $3)/mg;
# destination = $hilo (suffix '<hilo')
$source =~ s/^\s*\(([A-Z]+)<hilo\s+([^)]+)\)/\t($1\t\$hilo $2)/mg;
# source1 = $hilo (suffix '>hilo')
$source =~ s/^\s*\(([A-Z]+)>hilo\s+(\S+)\)/\t($1\t$2 \$hilo)/mg;
# source2 = $0 (suffix '>0')
$source =~ s/^\s*\(([A-Z+]+)>0\s+(\S+)\s+(\S+)\)/\t($1\t$2 \$0 $3)/mg;
# source1 = cc0 (suffix '>cc0')
$source =~ s/^\s*\(([A-Z0-9>+]+)>cc0\s+([^)]+)\)/\t($1\t\$cc0 $2)/mg;
# conditional jumps ('...+IF')
$source =~ s/^\s*\(([^+^ ]+)\+(IF[FT]*)\s+(\S+)\s+(\S+)\s+(\S+)\)/\t($1\t\$cc0 $3 $4)\n\t($2\t\$cc $5)/mg;
$source =~ s/^\s*\(([^+^ ]+)\+(IF[FT]*)\s+(\S+)\s+(\S+)\)/\t($1\t\$cc0 $3)\n\t($2\t\$cc0 $4)/mg;
# INOT done as IXOR with 1
$source =~ s/^\s*\(([A-Z]+)>1\s+([^)]+)\)/\t($1\t$2 1)/mg;
# always false comparisons (suffix 'cc00')
$source =~ s/^\s*\(([A-Z]+)cc00\s+[^)]+\)/\t($1\t\$cc0 0)/mg;
# destination of comparisons (suffix 'cc0')
$source =~ s/^\s*\(([A-Z]+)cc0\s+([^)]+)\)/\t($1\t\$cc0 $2)/mg;
# remove $ from $L...
$source =~ s/^(\s*\([^)]*)\$(L[^)]*\))/$1$2/mg;
# convert GOTO $31 to IGOTO $31
$source =~ s/^\s*\(GOTO\s+\$31\)/\t(IGOTO\t\$31)/mg;
# remove third argument from some instructions (suffix '-')
$source =~ s/^\s*\(([A-Z]+)\-\s+(\S+)\s+(\S+)\s+\S+\)/\t($1\t$2 $3)/mg;
# swap second and third argument from some instructions (suffix '=')
$source =~ s/^\s*\(([A-Z]+)\=\s+(\S+)\s+(\S+)\s+(\S+)\)/\t($1\t$2 $4 $3)/mg;
$source =~ s/^\s*\(([A-Z]+)\=\s+(\S+)\s+(\S+)\)/\t($1\t$2 $3)/mg;
# delete all arguments
$source =~ s/^\s*\(([A-Z]+)\~[^\)]*\)/\t(NOP)\t\t/mg;
# replace ICONSTANT d s1 s2 by IADD d s1 s2
$source =~ s/^\s*\(ICONSTANT(\s+\S+\s+\S+\s+\S+\))/\t(IADD$1/mg;
# replace IDIV(U) s1 s2 by IDIV(U) hilo s1 s2
$source =~ s/^\s*\((IDIVU?)\s+(\S+\s+\S+\))/\t($1\t\$hilo $2/mg;
# replace IDIV(U) $0 s1 s2 by IDIV(U) hilo s1 s2
$source =~ s/^\s*\((IDIVU?)\s+\$0\s+(\S+\s+\S+\))/\t($1\t\$hilo $2/mg;

print "done\n";

# remove the rest of .stabs
print "\tstabs... ";
$source =~ s/^(\.stabs .*)$/\t\t\t\t\t\t\;$1/mg;
print "done\n";

# remove other directives
print "\tdirectives... ";
$source =~ s/^(\s*\..*)$/\t\t\t\t\t\t\;$1/mg;
$source =~ s/^(\s*\..*)$/\t\t\t\t\t\t\;$1/mg;
print "done\n";

# handle all comments
print "\tcomments... ";
$source =~ s/^(\s*\#.*)$/\t\t\t\t\t\t\;$1/mg;
print "done\n";

# replace traps (branches to 1f, then a label '1:') with branches to a regular label (L1f_<no>)
$label_no = 0;
$source =~ s/^(\s*\(IF\s+[^\)]+\s+)1f(\).*?\n\s*\(LABEL\s+)_1\)/"$1L1f_${label_no}$2L1f_" . $label_no++ . ")"/msge;
#eval($1 . "L1f_" . $label_no . $2 . "L1f_" . $label_no++ . ")")/msge;

print OUT_FILE $source;

print "Translation ok.\n";

