#!/usr/bin/perl5

$repl{"\\.0fake"} = "fake0";
$repl{"\\.1fake"} = "fake1";
$repl{"\\.36\\)"} = "_36\)";
$repl{"\\.2\\)"} = "_2\)";
$repl{"\\.3\\)"} = "_3\)";
$repl{"\\.4\\)"} = "_4\)";
$repl{"\\.5\\)"} = "_5\)";
$repl{"\\.6\\)"} = "_6\)";
$repl{"\\.7\\)"} = "_7\)";
$repl{"\\.36 "} = "_36 ";
$repl{"\\.2 "} = "_2 ";
$repl{"\\.3 "} = "_3 ";
$repl{"\\.4 "} = "_4 ";
$repl{"\\.5 "} = "_5 ";
$repl{"\\.6 "} = "_6 ";
$repl{"\\.7 "} = "_7 ";



foreach $file (@ARGV) {

    open(IN_FILE, $file) || die "Cannot open file: $file";
    $source = join('', <IN_FILE>);
    close IN_FILE;
 
    foreach $key (keys %repl) {
	$source =~ s/$key/$repl{$key}/sg;
    }

    open(OUT_FILE, ">$file") || die "Cannot open file: $file";
    print OUT_FILE $source;
    close OUT_FILE;
}
