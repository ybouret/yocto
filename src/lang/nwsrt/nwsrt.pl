use Algorithm::Networksort;
use Algorithm::Networksort::Best qw(:all);

for (my $i = 2; $i <= 8; $i++) {
    my $inputs = $i;
    my $algorithm = "bosenelson";

    #
    # Generate the sorting network (a list of comparators).
    #
    my $nw = Algorithm::Networksort->new(inputs =>$inputs,algorithm => $algorithm);
    #$nw->formats([ " swap(%d,%d)," ]);

    #
    # Print the comparator list using the default format,
    # and print a graph of the list.
    #
    #print "$i", "\n";
    print "$i-", $nw->title(), "\n";
    print $nw->formatted(),"\n\n";

}

for (my $i = 9; $i <= 24; $i++)
{
    my $inputs = $i;

    #
    # First find if any networks exist for the size you want.
    #
    my @nwkeys = nw_best_names($inputs);

    #
    # For each sorting network, show the comparators.
    #
    for my $name (@nwkeys)
    {
        my $nw = nwsrt_best(name => $name);

        #
        # Print the list, and print the graph of the list.
        #
        print $nw->title(), "\n";
        print $nw->formatted(),"\n\n";
    }
}

for (my $i = 25; $i <= 32; $i++) {
    my $inputs = $i;
    my $algorithm = "bosenelson";

    #
    # Generate the sorting network (a list of comparators).
    #
    my $nw = Algorithm::Networksort->new(inputs =>$inputs,algorithm => $algorithm);
    #$nw->formats([ " swap(%d,%d)," ]);

    #
    # Print the comparator list using the default format,
    # and print a graph of the list.
    #
    #print "$i", "\n";
    print "$i-", $nw->title(), "\n";
    print $nw->formatted(),"\n\n";

}
