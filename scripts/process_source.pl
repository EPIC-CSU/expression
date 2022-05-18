#!/usr/bin/perl5

$ARGV[0] =~ /^(.*)\.s$/;
$FILE_BASE = $1;

open (IN_FILE, "$ARGV[0]") || die "Cannot open input file";
open (OUT_PROCS, ">$FILE_BASE.procs") || die "Cannot open output file";
open (OUT_DEFS, ">$FILE_BASE.defs") || die "Cannot open output file";

$source = join('', <IN_FILE>);

##############################################

# uncomment some keywords + add paranthesis
$source =~ s/;PROLOGUE/(PROLOGUE)\n(/g;

#$source =~ s/;EPILOGUE/)\n(EPILOGUE)\n(/g;
$source =~ s/;EPILOGUE/(EPILOGUE)\n(/g;

#$source =~ s/;END PROLOGUE/)\n(END PROLOGUE)\n(/g;
$source =~ s/;END PROLOGUE/)\n(END PROLOGUE)/g;

$source =~ s/;END EPILOGUE/)\n(END EPILOGUE)/g;

$procs = "";

##############################################

# get procs and their bodies
while ($source =~ /\n(\(LOCATION \d+\)\n\s*\(PROC_BEGIN\s+_(\S+).*?END OF \2\s*\n)/sg) {
    $procs .= "$1\n\n";
}

# delete defs and decls from procs bodies
$procs =~ s/\(DEF .*?\n//g;
$procs =~ s/\(DECL .*?\n//g;

# put an open paranthesis after the first BEGIN SCOPE and before the last END_SCOPE for every procedure
$procs =~ s/(\(PROC_BEGIN\s+_(\S+).*?)(\(BEGIN_SCOPE\))(.*)(\(END_SCOPE\))(.*?END OF \2\s*\n)/$1)\n\3\n\($4\)\n$5\n($6/sg;

# put paranthesis between END PROLOGUE and BEGIN SCOPE and between END SCOPE and EPILOGUE
#procs =~ s/(\(END PROLOGUE

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

#handle consecutive GOTOs
$procs =~ s/(\s*\(GOTO\s+\w+\)\s*\n)(\s*\(GOTO\s+\w+\)\s*\n)*/$1/sg;

#evaluate simple expressions like (1+2*3-2)...

$procs =~ s/\((\s*\d+(\s*[\+\-\*]\s*\d+)+\s*)\)/eval($1)/sge;

print OUT_PROCS $procs;

##############################################

# get text till first proc
$source =~ /^(.*?PROC_BEGIN\s+_\S+\s*\n)/s;
print OUT_DEFS "$1\n";

# get text from inside and after a procedure
while ($source =~ /\G(.*?\n)(\)\s*;\s*END OF.*?\(PROC_BEGIN\s+_\S+\s*\n)/sg) {
    $inside_proc = $1;
    $after_proc = $2;

    # look for defs and decls inside proc
    while ($inside_proc =~ /(.*?\n)/g) {
        $line = $1;

        if (($line =~ /^\(DEF .*?\n/) || ($line =~ /^\(DECL .*?\n/)) {
            print OUT_DEFS $line;
        }
    }
    print OUT_DEFS "\n$after_proc\n";
}

# get text from inside and after last procedure
$source =~ /.*PROC_BEGIN\s+_\S+\s*\n(.*?\n)(\)\s*;\s*END OF.*?)$/s;
    $inside_proc = $1;
    $after_proc = $2;

    # look for defs and decls inside procs
    while ($inside_proc =~ /(.*?\n)/g) {
        $line = $1;

        if (($line =~ /^\(DEF .*?\n/) || ($line =~ /^\(DECL .*?\n/)) {
            print OUT_DEFS $line;
        }
    }
print OUT_DEFS "\n$after_proc";

##############################################
