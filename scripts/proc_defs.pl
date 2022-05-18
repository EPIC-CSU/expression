#!/usr/bin/perl5

$ARGV[0] =~ /^(.*)\.[sx]$/;
$FILE_BASE = $1;

open (IN_FILE, "$ARGV[0]") || die "Cannot open input file";
open (OUT_PROCS, ">$FILE_BASE.procs") || die "Cannot open output file";
open (OUT_DEFS, ">$FILE_BASE.defs") || die "Cannot open output file";

$source = join('', <IN_FILE>);

##############################################

# remove all \000 occurences
$source =~ s/\\000//g;

# uncomment some keywords + add paranthesis
$source =~ s/;PROLOGUE/(PROLOGUE)\n(/g;

#$source =~ s/;EPILOGUE/)\n(EPILOGUE)\n(/g;
$source =~ s/;EPILOGUE/(EPILOGUE)\n(/g;

#$source =~ s/;END PROLOGUE/)\n(END PROLOGUE)\n(/g;
$source =~ s/;END PROLOGUE/)\n(END PROLOGUE)/g;

$source =~ s/;END EPILOGUE/)\n(END EPILOGUE)/g;

#get rid of (CALL ___main ...)
$source =~ s/\s*\(CALL\s+___main\([^\)]*\)\)//g;
$source =~ s/\s*\(CALL\s+___gccmain[^\)]*\)/\t\t\t/g;

$procs = "";

##############################################

# get procs and their bodies
#while ($source =~ /\n(\(LOCATION \d+\).*?\n\s*\(PROC_BEGIN\s+_(\S+).*?END OF [_]??\2.*?\n)/sg) {
while ($source =~ /(\s*\(PROC_BEGIN\s+_(\S+).*?END OF [_]??\2.*?\n)/sg) {
    $procs .= "$1\n\n";
}

# delete defs and decls from procs bodies
$procs =~ s/\(DEF .*?\n//g;
$procs =~ s/\(DECL .*?\n//g;

# put an open paranthesis after the first BEGIN SCOPE and before the last END_SCOPE for every procedure
$procs =~ s/(\(PROC_BEGIN\s+_(\S+).*?)(\(BEGIN_SCOPE\))(.*)(\(END_SCOPE\))(.*?END OF \2\s*\n)/$1)\n\3\n\($4\)\n$5\n($6/sg;

# put paranthesis between END PROLOGUE and BEGIN SCOPE and between END SCOPE and EPILOGUE
#procs =~ s/(\(END PROLOGUE

# bring LOCATIONs before LABELs
#while ($procs =~ s/(\(LABEL.*?\n)[\s\n]*?(\(LOCATION.*?\n)/$2\t$1/g) {};

@lines = split(/[\n\r]/, $procs);
$procs = "";
while ($#lines > -1) {
    $line = shift @lines;

    if (($line =~ /\s*\(LABEL/) && ($#lines > -1)) {
	$labels = $line . "\n";
	$rest = "";

	if ($#lines == -1) {
	    last;
	}
	$line = shift @lines;

	while (1) {
	    if ($line =~ /^\s*\(LABEL.*$/) {
		$labels .= $line . "\n";
	    } elsif ($line =~ /^\s*\(LOCATION.*$/) {
		$rest .= $line . "\n";
	    } elsif ($line =~ /^\s*;.*$/) {
		$rest .= $line . "\n";
	    } else {
		$procs .= $labels;
		$procs .= $rest;
		$procs .= $line . "\n";

		last;
	    }
	    
	    if ($#lines == -1) {
		last;
	    }    
	    $line = shift @lines;
	}
    } else {
	$procs .= $line . "\n";
    }
}

# add BEGIN_SCOPE and END_SCOPE where needed and not already there

$rest = $procs;

$procs = "";

while ($rest =~ /^(.*?\(END PROLOGUE\))(.*?)(\(EPILOGUE\))(.*)$/sg) {

    local($before, $body, $after, $todo) = ($1, $2, $3, $4);

    #handle consecutive LABELs
    while ($body =~ s/(\s*\(LABEL\s+(\w+)\)\s*\n)(\s*\(LABEL\s+(\w+)\)\s*\n)/$1/s) {
	$from = $4;
	$to = $2;
	$body =~ s/(\s*\(GOTO\s+)${from}(\)\s*\n)/$1${to}$2/sg;
        $body =~ s/(\s*\(IF\s+\S+\s+)${from}(\)\s*\n)/$1${to}$2/sg;
    }

    if ($body =~ /\(BEGIN_SCOPE\)/) {
	$procs .= "$before\n(\n$body\n)\n$after";
    } else {
	$procs .= "$before\n(\n)\n(BEGIN_SCOPE)\n($body\n)\n(END_SCOPE)\n(\n)\n$after";
    }

    $rest = $todo;
}

$procs .= $rest;

#handle consecutive LABELs outside procedures
while (1) {
    if ($procs =~ s/(\s*\(LABEL\s+([\w\$]+)\)[^\n]*)\n\s*\(LABEL\s+([\w\$]+)\)/$1/s) {
	$from = $3;
	$to = $2;

	$procs =~ s/\((GOTO\s+)${from}\)/($1${to})/sg;
        $procs =~ s/\((IF\s+\S+\s+)${from}\)/($1${to})/sg;
    } else {
	last;
    }
}

#handle consecutive GOTOs
$procs =~ s/(\s*\(GOTO\s+\w+\)\s*\n)(\s*\(GOTO\s+\w+\)\s*\n)*/$1/sg;

#evaluate simple expressions like (1+2*3-2)...
$procs =~ s/\((\s*\d+(\s*[\+\-\*]\s*\d+)+\s*)\)/eval($1)/sge;

#replace ___swbuf with _putc
$procs =~ s/___swbuf/_putc/sg;

print OUT_PROCS $procs;

##############################################

# remove all LABELs
$source =~ s/\(LABEL\s+[^\)]+\)//sg;

if ($source =~ /.*PROC_BEGIN.*/) {

    # get text from before and defs inside a procedure
    while ($source =~ /\G(.*?\(PROC_BEGIN\s+_(\S+))(.*?)(\n\)\s*;\s*END OF [_]??\2.*?\n)/sg) {

	$before_proc = $1;
	$inside_proc = $3;
	$end_proc = $4;

	print OUT_DEFS "\n$before_proc\n";

	# look for defs and decls inside proc
	while ($inside_proc =~ /(.*?\n)/g) {
	    $line = $1;

	    if (($line =~ /^\(DEF .*?\n/) || ($line =~ /^\(DECL .*?\n/)) {
                print OUT_DEFS $line;
            }
        }

        print OUT_DEFS "\n$end_proc\n";
    }

    # get text after last procedure

    $source =~ /.*END OF \S+.*?\n(.*?)$/s;
    print OUT_DEFS $1;

} else {  # no procedure bodies
    print OUT_DEFS $source;

}
##############################################
