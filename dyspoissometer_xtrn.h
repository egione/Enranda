/*
Dyspoissometer
Copyright 2016 Russell Leidich
http://dyspoissonism.blogspot.com

This collection of files constitutes the Dyspoissometer Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Dyspoissometer Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Dyspoissometer Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Dyspoissometer Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
extern u8 dyspoissometer_init(u32 build_break_count, u32 build_feature_count);
extern void *dyspoissometer_free(void *base);
extern DYSPOISSOMETER_NUMBER *dyspoissometer_number_list_malloc(DYSPOISSOMETER_UINT number_idx_max);
extern DYSPOISSOMETER_UINT *dyspoissometer_uint_list_malloc(DYSPOISSOMETER_UINT uint_idx_max);
extern void dyspoissometer_uint_list_copy(DYSPOISSOMETER_UINT uint_count_minus_1, DYSPOISSOMETER_UINT uint_idx0_min, DYSPOISSOMETER_UINT uint_idx1_min, DYSPOISSOMETER_UINT *uint_list0_base, DYSPOISSOMETER_UINT *uint_list1_base);
extern void *dyspoissometer_uint_list_list_free(DYSPOISSOMETER_UINT list_idx_max, DYSPOISSOMETER_UINT **list_list_base);
extern void dyspoissometer_uint_list_zero(DYSPOISSOMETER_UINT uint_idx_max, DYSPOISSOMETER_UINT *uint_list_base);
extern DYSPOISSOMETER_UINT *dyspoissometer_uint_list_malloc_zero(DYSPOISSOMETER_UINT uint_idx_max);
extern void dyspoissometer_uint_list_sort(DYSPOISSOMETER_UINT uint_idx_max, DYSPOISSOMETER_UINT *uint_list0_base, DYSPOISSOMETER_UINT *uint_list1_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_logfreedom_sparse_get(DYSPOISSOMETER_UINT *freq_list0_base, DYSPOISSOMETER_UINT *freq_list1_base, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_NUMBER dyspoissometer_logfreedom_dense_get(DYSPOISSOMETER_UINT freq_max_minus_1, DYSPOISSOMETER_UINT freq_min, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_UINT *pop_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_dyspoissonism_get(DYSPOISSOMETER_NUMBER logfreedom, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_NUMBER dyspoissometer_sparsity_get(DYSPOISSOMETER_NUMBER logfreedom, DYSPOISSOMETER_NUMBER logfreedom_max);
extern u8 dyspoissometer_freq_list_update_autoscale(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT *mask_count_implied_base, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT mask_max);
extern void dyspoissometer_freq_list_get(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_UINT dyspoissometer_freq_max_minus_1_get(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_UINT dyspoissometer_pop_list_get(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT freq_max_minus_1, DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_UINT *pop_list_base);
extern DYSPOISSOMETER_UINT *dyspoissometer_pop_list_init(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT *freq_max_minus_1_base, DYSPOISSOMETER_UINT *h0_base, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_UINT *dyspoissometer_pop_list_obtuse_init(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT freq_max_minus_1, DYSPOISSOMETER_UINT *h0_base, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_NUMBER dyspoissometer_poisson_term_get(DYSPOISSOMETER_UINT freq, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max);
extern DYSPOISSOMETER_NUMBER dyspoissometer_logfreedom_max_get(u64 iteration_max, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max, u64 *random_seed_base);
extern void dyspoissometer_mask_list_pseudorandom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT mask_max, u64 *random_seed_base);
extern void dyspoissometer_number_list_sort(DYSPOISSOMETER_UINT number_idx_max, DYSPOISSOMETER_NUMBER *number_list_base0, DYSPOISSOMETER_NUMBER *number_list_base1);
extern DYSPOISSOMETER_NUMBER dyspoissometer_logfreedom_median_get(DYSPOISSOMETER_UINT iteration_max, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max, u64 *random_seed_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_uint_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u32 mask_max, u16 *u16_list_base, u8 *u24_list_base, u32 *u32_list_base, u8 *u8_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_uint_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u16 *u16_list_base, u8 *u24_list_base, u32 *u32_list_base, u8 *u8_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_uint_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u16 *u16_list_base, u8 *u24_list_base, u32 *u32_list_base, u8 *u8_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u16_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u16 mask_max, u16 *u16_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u16_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u16 *u16_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u16_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u16 *u16_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u24_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u32 mask_max, u8 *u24_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u24_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u8 *u24_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u24_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u8 *u24_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u32_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u32 mask_max, u32 *u32_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u32_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u32 *u32_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u32_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u32 *u32_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u8_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 mask_max, u8 *u8_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u8_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u8 *u8_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_u8_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u8 *u8_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_kernel_density_expected_fast_get(DYSPOISSOMETER_UINT mask_idx_max);
extern DYSPOISSOMETER_NUMBER dyspoissometer_kernel_density_expected_slow_get(DYSPOISSOMETER_UINT mask_idx_max);
extern DYSPOISSOMETER_UINT dyspoissometer_kernel_size_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_kernel_density_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT kernel_size);
extern DYSPOISSOMETER_NUMBER dyspoissometer_kernel_skew_get(DYSPOISSOMETER_NUMBER kernel_density, DYSPOISSOMETER_NUMBER kernel_density_expected);
extern DYSPOISSOMETER_NUMBER dyspoissometer_mibr_expected_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max);
extern void dyspoissometer_uint_idx_list_sort(DYSPOISSOMETER_UINT *idx_list_base0, DYSPOISSOMETER_UINT *idx_list_base1, DYSPOISSOMETER_UINT uint_idx_max, DYSPOISSOMETER_UINT *uint_list_base0, DYSPOISSOMETER_UINT *uint_list_base1);
extern u8 dyspoissometer_mibr_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT *mibr_base);
extern DYSPOISSOMETER_NUMBER dyspoissometer_skew_from_mibr_get(DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_UINT mibr);
extern DYSPOISSOMETER_UINT dyspoissometer_mibr_from_skew_get(DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_NUMBER probability);
