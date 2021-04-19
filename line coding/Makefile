gg = g++ -std=c++14
lvl_en = lvl_encoder.cpp
lvl_de = lvl_decoder.cpp
inv_en = inv_encoder.cpp
inv_de = inv_decoder.cpp
in = input.txt
out = output.txt
d_out = decode_output.txt

nrz_level_encode: $(lvl_en) $(in)
	$(gg) $(lvl_en) -o a && a < $(in) > $(out)
nrz_level_decode: $(lvl_de) $(out)
	 $(gg) $(lvl_de) -o a && a < $(out) > $(d_out)
nrz_inv_encode:  $(inv_en) $(in)
	 $(gg)  $(inv_en) -o a && a < $(in) > $(out)
nrz_inv_decode:  $(inv_de) $(out)
	 $(gg)  $(inv_de) -o a && a < $(out) > $(d_out)