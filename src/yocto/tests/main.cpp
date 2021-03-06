#include "yocto/utest/driver.hpp"

using namespace yocto;


YOCTO_UNIT_TEST_INIT(200)
{

    YOCTO_UNIT_TEST_DECL(platform);
    YOCTO_UNIT_TEST_DECL(mutex);
    YOCTO_UNIT_TEST_DECL(round);
    YOCTO_UNIT_TEST_DECL(unroll);
    YOCTO_UNIT_TEST_DECL(wtime);
    YOCTO_UNIT_TEST_DECL(thread);
    YOCTO_UNIT_TEST_DECL(pool);
    YOCTO_UNIT_TEST_DECL(list);
    YOCTO_UNIT_TEST_DECL(at_exit);
    YOCTO_UNIT_TEST_DECL(singleton);
    YOCTO_UNIT_TEST_DECL(global);
    YOCTO_UNIT_TEST_DECL(slice);
    YOCTO_UNIT_TEST_DECL(carver);
    YOCTO_UNIT_TEST_DECL(pooled);
    YOCTO_UNIT_TEST_DECL(string);
    YOCTO_UNIT_TEST_DECL(hexdump);
    YOCTO_UNIT_TEST_DECL(type_traits);
    YOCTO_UNIT_TEST_DECL(functor);
    YOCTO_UNIT_TEST_DECL(container);
    YOCTO_UNIT_TEST_DECL(sequence);
    YOCTO_UNIT_TEST_DECL(primality);
    YOCTO_UNIT_TEST_DECL(primgen);
    YOCTO_UNIT_TEST_DECL(hash);
    YOCTO_UNIT_TEST_DECL(hcol);
    YOCTO_UNIT_TEST_DECL(streams);
    YOCTO_UNIT_TEST_DECL(ppty);
    YOCTO_UNIT_TEST_DECL(uuid);
    YOCTO_UNIT_TEST_DECL(env);
    YOCTO_UNIT_TEST_DECL(scroll);
    YOCTO_UNIT_TEST_DECL(auto_scroll);
    YOCTO_UNIT_TEST_DECL(utf8);
    YOCTO_UNIT_TEST_DECL(slab);
    YOCTO_UNIT_TEST_DECL(embed);
    YOCTO_UNIT_TEST_DECL(map);
    YOCTO_UNIT_TEST_DECL(set);
    YOCTO_UNIT_TEST_DECL(locate);
    YOCTO_UNIT_TEST_DECL(shared_ptr);
    YOCTO_UNIT_TEST_DECL(alias_ptr);
    YOCTO_UNIT_TEST_DECL(soft_ptr);
    YOCTO_UNIT_TEST_DECL(linked_ptr);
    YOCTO_UNIT_TEST_DECL(stock);
    YOCTO_UNIT_TEST_DECL(frag_layout);
    YOCTO_UNIT_TEST_DECL(frag_block);
    YOCTO_UNIT_TEST_DECL(frag_queue);
    YOCTO_UNIT_TEST_DECL(bitio);
    YOCTO_UNIT_TEST_DECL(xarray);
    YOCTO_UNIT_TEST_DECL(dict);
    YOCTO_UNIT_TEST_DECL(catalog);
    YOCTO_UNIT_TEST_DECL(c_array);
    YOCTO_UNIT_TEST_DECL(raw_file);
    YOCTO_UNIT_TEST_DECL(module);
    YOCTO_UNIT_TEST_DECL(sort);
    YOCTO_UNIT_TEST_DECL(duration);
    YOCTO_UNIT_TEST_DECL(eta);
    YOCTO_UNIT_TEST_DECL(index);
    YOCTO_UNIT_TEST_DECL(records);
    YOCTO_UNIT_TEST_DECL(intrusive_ptr);
    YOCTO_UNIT_TEST_DECL(sorted_vec);
    YOCTO_UNIT_TEST_DECL(strconv);
    YOCTO_UNIT_TEST_DECL(codec);
    YOCTO_UNIT_TEST_DECL(base64);
    YOCTO_UNIT_TEST_DECL(heap);
    YOCTO_UNIT_TEST_DECL(place);
    YOCTO_UNIT_TEST_DECL(rcpack);
    YOCTO_UNIT_TEST_DECL(rcload);
    YOCTO_UNIT_TEST_DECL(rand32);
    YOCTO_UNIT_TEST_DECL(clist);
    YOCTO_UNIT_TEST_DECL(cached);
    YOCTO_UNIT_TEST_DECL(hlist);
    YOCTO_UNIT_TEST_DECL(labelled);
    YOCTO_UNIT_TEST_DECL(bswap);
    YOCTO_UNIT_TEST_DECL(log_round);
    YOCTO_UNIT_TEST_DECL(utils);
    YOCTO_UNIT_TEST_DECL(threads);
    YOCTO_UNIT_TEST_DECL(scan);
    YOCTO_UNIT_TEST_DECL(wd);
    YOCTO_UNIT_TEST_DECL(unique);
    YOCTO_UNIT_TEST_DECL(remove_if);
    YOCTO_UNIT_TEST_DECL(cslot);
    YOCTO_UNIT_TEST_DECL(vslot);
    YOCTO_UNIT_TEST_DECL(xslot);
    YOCTO_UNIT_TEST_DECL(dualmap);
    YOCTO_UNIT_TEST_DECL(dualset);
    YOCTO_UNIT_TEST_DECL(dualset2);
    YOCTO_UNIT_TEST_DECL(multi_map);
    YOCTO_UNIT_TEST_DECL(mlock);
    YOCTO_UNIT_TEST_DECL(pages);
    YOCTO_UNIT_TEST_DECL(bmove);
    YOCTO_UNIT_TEST_DECL(qsort);
    YOCTO_UNIT_TEST_DECL(sort_perf);
    YOCTO_UNIT_TEST_DECL(ysort);
    YOCTO_UNIT_TEST_DECL(bzset);
    YOCTO_UNIT_TEST_DECL(clock);
    YOCTO_UNIT_TEST_DECL(multi_arrays);
    YOCTO_UNIT_TEST_DECL(endian);

    YOCTO_UNIT_TEST_DECL(rle);
    YOCTO_UNIT_TEST_DECL(unrle);

    YOCTO_UNIT_TEST_DECL(huffenc);
    YOCTO_UNIT_TEST_DECL(huffdec);

    YOCTO_UNIT_TEST_DECL(bwt);
    YOCTO_UNIT_TEST_DECL(bwt16);

    YOCTO_UNIT_TEST_DECL(file_size);
    YOCTO_UNIT_TEST_DECL(wallpaper);

    YOCTO_UNIT_TEST_DECL(condition);
    YOCTO_UNIT_TEST_DECL(barrier);

    YOCTO_UNIT_TEST_DECL(binary);

    YOCTO_UNIT_TEST_DECL(lexicon);
    YOCTO_UNIT_TEST_DECL(comb);
    YOCTO_UNIT_TEST_DECL(comb_perf);
    YOCTO_UNIT_TEST_DECL(comb_io);
    YOCTO_UNIT_TEST_DECL(dispatch);
    YOCTO_UNIT_TEST_DECL(ipower);

    YOCTO_UNIT_TEST_DECL(tokenizer);
    YOCTO_UNIT_TEST_DECL(tuple);
    YOCTO_UNIT_TEST_DECL(factory);

    YOCTO_UNIT_TEST_DECL(kChunk);
    YOCTO_UNIT_TEST_DECL(kArena);
    YOCTO_UNIT_TEST_DECL(kBlocks);
    YOCTO_UNIT_TEST_DECL(kObject);
    YOCTO_UNIT_TEST_DECL(kObjPerf);

    YOCTO_UNIT_TEST_DECL(mkdir);

    YOCTO_UNIT_TEST_DECL(slots);
    YOCTO_UNIT_TEST_DECL(slots2D);
    YOCTO_UNIT_TEST_DECL(matrix);


    YOCTO_UNIT_TEST_DECL(xtensor);

    YOCTO_UNIT_TEST_DECL(variadic);
    YOCTO_UNIT_TEST_DECL(mph);
    YOCTO_UNIT_TEST_DECL(net_string);
    YOCTO_UNIT_TEST_DECL(glossary);
    YOCTO_UNIT_TEST_DECL(htree);

    YOCTO_UNIT_TEST_DECL(mtf);
    YOCTO_UNIT_TEST_DECL(unmtf);



    YOCTO_UNIT_TEST_DECL(SIMD);
    YOCTO_UNIT_TEST_DECL(server);

    YOCTO_UNIT_TEST_DECL(ansi);
    YOCTO_UNIT_TEST_DECL(null_device);
    YOCTO_UNIT_TEST_DECL(null_device_cpp);

    YOCTO_UNIT_TEST_DECL(multi_set);

    YOCTO_UNIT_TEST_DECL(xbwt);
    YOCTO_UNIT_TEST_DECL(TreeFS);
    YOCTO_UNIT_TEST_DECL(randomized);
    YOCTO_UNIT_TEST_DECL(randQC);
    YOCTO_UNIT_TEST_DECL(alea);
    YOCTO_UNIT_TEST_DECL(gaussian);


    YOCTO_UNIT_TEST_DECL(netsort);
    YOCTO_UNIT_TEST_DECL(nwsrt);

    YOCTO_UNIT_TEST_DECL(isqrt);
    YOCTO_UNIT_TEST_DECL(vecops);

    YOCTO_UNIT_TEST_DECL(HList);

}
YOCTO_UNIT_TEST_EXEC()

