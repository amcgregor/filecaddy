$dirname = $ARGV[0];
if ($#ARGV > 1) {
    $arg2 = $ARGV[1];
}
else {
    $arg2 = "x";
}

$filename1 = $dirname . "/BuildNumber.h";
$filename2 = $dirname . "/BuildNumber.make";

if ($arg2 eq "reset") {
    print "Resetting build number\n";
    $number = 1;
}
else {
    open(INFILE, $filename1) || die "Can't open $filename for read";

    while (<INFILE>) {
	if (/\#define BUILD_NUMBER ([\d]+)/) {
	    $number = $1 + 1;
	}
    }

    close(INFILE);
}

open(OUTFILE, ">$filename1") || die "Can't open $filename1 for write";
print OUTFILE "#define BUILD_NUMBER $number\n";
close(OUTFILE);

open(OUTFILE, ">$filename2") || die "Can't open $filename2 for write";
print OUTFILE "BuildNumber=$number\n";
close(OUTFILE);
