project(spuc)

# Required cmake version
cmake_minimum_required(VERSION 2.6.0)


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

set(SRCS
	array/array1d_functions.cpp
	array/array2d_functions.cpp
	comm/baud_eq_env.cpp
	comm/bch.cpp
	comm/bped.cpp
	comm/bpsk_ber_test.cpp
	comm/carrier_nco.cpp
	comm/commfunc.cpp
	comm/convcode.cpp
	comm/crc.cpp
	comm/data_conv_encoder.cpp
	comm/dvb_conv_encoder.cpp
	comm/egolay.cpp
	comm/fading_channel.cpp
	comm/gf.cpp
	comm/gfx.cpp
	comm/hammcode.cpp
	comm/modulator.cpp
	comm/ofdm_data_encoder.cpp
	comm/punct_convcode.cpp
        #comm/qam_conv_decoder.cpp
	comm/qam_mod.cpp
	comm/qam_soft_decision.cpp
	comm/qam_tx.cpp
	comm/qpsk.cpp
	comm/qpsk_ber_test.cpp
	comm/qpsk_discriminators.cpp
	comm/quad_data.cpp
	comm/raised_cosine.cpp
	comm/rec_syst_conv_code.cpp
	comm/reed_solomon.cpp
	comm/sequence.cpp
	comm/sim_qam.cpp
	comm/sim_qpsk.cpp
	comm/turbo.cpp
	comm/viterbi.cpp
	equalizers/lms_dfe.cpp
	equalizers/mlsd.cpp
	equalizers/rls.cpp
	functions/idft.cpp
	functions/ifft.cpp
	functions/misc.cpp
	functions/scalfunc.cpp
	functions/spuc_math.cpp
        functions/window.cpp
	generic/binary.cpp
	generic/bool_nco.cpp
	generic/butterworth_fir.cpp
	generic/cic.cpp
	generic/cordic.cpp
	generic/create_remez_lpfir.cpp
        #generic/elliptic.cpp
	generic/fir.cpp
	generic/gaussian_fir.cpp
	generic/lib_inst.cpp
	generic/ls_fir.cpp
	generic/max_pn.cpp
	generic/nco.cpp
	generic/noise.cpp
	generic/random.cpp
	generic/remez_fir.cpp
	generic/remezf.cpp
	generic/sigdel.cpp
	generic/sum_and_dump.cpp
	generic/vco.cpp
	matrix/cholesky.cpp
	matrix/converters.cpp
	matrix/det.cpp
	matrix/eigen.cpp
	matrix/fastmath.cpp
	matrix/inv.cpp
	matrix/ls_solve.cpp
	matrix/lu.cpp
	matrix/matrix.cpp
	matrix/specmat.cpp
	matrix/svd.cpp
	matrix/vector.cpp
        #specializations/fnco.cpp
	specializations/scic.cpp
        #utility/matlab_engine.cpp
    utility/spucassert.cpp
)


set(HDRS
          array/array1d.h
          array/array2d.h
          array/array3d.h
          array/tnt_cholesky.h
          array/tnt_lu.h
          array/tnt_qr.h
          array/tnt_transv.h
          array/tnt_triang.h
          array/tnt_trisolve.h
          comm/base_demod.h
          comm/baud_eq_env.h
          comm/bch.h
          comm/bit_scrambler.h
          comm/bped.h
          comm/bpsk_ber_test.h
          comm/bpsk_dd_phase.h
          comm/bpsk_quadricorrelator.h
          comm/carrier_nco.h
          comm/commfunc.h
          comm/conv_interleaver.h
          comm/convcode.h
          comm/cp_afc.h
          comm/crc.h
          comm/data_conv_encoder.h
          comm/dd_symbol.h
          comm/discriminators.h
          comm/dqpsk.h
          comm/dvb_conv_encoder.h
          comm/egolay.h
          comm/fading_channel.h
          comm/gf.h
          comm/gfx.h
          comm/hammcode.h
          comm/interleave.h
          comm/modulator.h
          comm/nda_symbol.h
          comm/ofdm_data_encoder.h
          comm/punct_convcode.h
          #comm/qam_conv_decoder.h
          comm/qam_mod.h
          comm/qam_soft_decision.h
          comm/qam_tx.h
          comm/qpsk.h
          comm/qpsk_ber_test.h
          comm/qpsk_dd_phase.h
          comm/qpsk_discriminators.h
          comm/qpsk_quadricorrelator.h
          comm/qpsk_rcfd.h
          comm/quad_data.h
          comm/raised_cosine.h
          comm/rec_syst_conv_code.h
          comm/reed_solomon.h
          comm/sequence.h
          comm/sim_qam.h
          comm/sim_qpsk.h
          comm/slicer_type.h
          comm/turbo.h
          comm/viterbi.h
          equalizers/lms_dfe.h
          equalizers/mle.h
          equalizers/mlsd.h
          equalizers/rls.h
          functions/idft.h
          functions/misc.h
          functions/scalfunc.h
          functions/spuc_math.h
          generic/allpass_1.h
          generic/a_d.h
          generic/allpass_halfband.h
          generic/allpass_iir.h
          generic/auto_corr.h
          generic/binary.h
          generic/bit.h
          generic/bool_nco.h
          generic/burg.h
          generic/butterworth.h
          generic/cfir.h
          generic/chebyshev.h
          generic/cic.h
          generic/circ.h
          generic/complex.h
          generic/complex_iu.h
          generic/complex_u.h
          generic/cordic.h
          generic/cplxfft.h
          generic/delay.h
          #generic/elliptic.h
          generic/farrow.h
          generic/fir.h
          generic/fir_adapt.h
          generic/fir_decim.h
          generic/fir_interp.h
          generic/fir_sparse_coef.h
          generic/iir_1st.h
          generic/iir_2nd.h
          generic/iir_2ndhpf.h
          generic/iir_comb.h
          generic/iir_df.h
          generic/iir_hp1.h
          generic/iir_hpc.h
          generic/iir_hpf.h
          generic/iir_lpf.h
          generic/iir_shelf.h
          generic/int_s.h
          generic/int_u.h
          generic/lagrange.h
          generic/levdur.h
          generic/loop_filter.h
          generic/max_pn.h
          generic/nco.h
          generic/noise.h
          generic/random.h
          generic/remez_fir.h
          generic/running_average.h
          generic/running_sum.h
          generic/rv_stat.h
          generic/sigdel.h
          generic/sint.h
          generic/spuc.h
          generic/sum_and_dump.h
          generic/uint.h
          generic/vco.h
          matrix/array.h
          matrix/cblas.h
          matrix/cholesky.h
          matrix/converters.h
          matrix/det.h
          matrix/eigen.h
          matrix/fastmath.h
          matrix/inv.h
          matrix/lapack.h
          matrix/ls_solve.h
          matrix/lu.h
          matrix/matfunc.h
          matrix/matrix.h
          matrix/specmat.h
          matrix/spucconfig.h
          matrix/svd.h
          matrix/toeplitz.h
          matrix/vector.h
          specializations/allpass_1a.h
          specializations/allpass_1aq1.h
          specializations/allpass_1aq2.h
          specializations/cmplx_csd_allpass_halfband.h
          specializations/csd_allpass_halfband.h
          specializations/double_allpass_iir.h
          specializations/double_lagrange.h
          specializations/fbool_nco.h
          #specializations/fnco.h
          specializations/scic.h
          #utility/debugf.h
          #utility/debug.h
          #utility/debugfc.h
          #utility/matlab_engine.h
          utility/spucassert.h
          #utility/spucmex.h
)


set(UIS

)

set(RCS


)


qt4_wrap_cpp(MOC_SRCS ${HDRS})
qt4_wrap_ui(UI_HDRS ${UIS})
qt4_add_resources(RC_SRCS ${RCS})



include_directories(
    matrix
    generic
    specializations
    utility
    comm
    functions
    equalizers
    array
    ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTSVG_INCLUDE_DIR}
)

link_directories(
    ${CMAKE_BINARY_DIR}/lib
)

add_library(spuc
  STATIC
  ${SRCS}
  ${HDRS}
  ${UI_HDRS}
  ${RC_SRCS}
  ${MOC_SRCS}
  ${QM_FILES}
)

target_link_libraries( spuc
      ${QT_LIBRARIES}
)
