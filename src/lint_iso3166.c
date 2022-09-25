/*
 * GS1 Syntax Dictionary. Copyright (c) 2022 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file lint_iso3166.c
 *
 * @brief The `iso3166` linter ensures that the data represents a valid ISO
 * 3166 "num-3" country code.
 *
 * @remark The three-digit country codes are defined by [ISO 3166-1: Codes for the representation of names of countries and their subdivisions - Part 1: Country code](https://www.iso.org/standard/72482.html) as the "num-3" codes.
 *
 */


#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"


/*
 * Include a header containing a replacement lookup function, if we are told
 * to.
 *
 */
#ifdef GS1_LINTER_CUSTOM_ISO3166_LOOKUP_H
#define xstr(s) str(s)
#define str(s) #s
#include xstr(GS1_LINTER_CUSTOM_ISO3166_LOOKUP_H)
#endif


/**
 * Used to validate that an AI component is an ISO 3166 "num-3" country
 * code.
 *
 * @note The default lookup function provided by this linter is a linear search
 *       over a static list this is maintained in this file.
 * @note To enable this linter to hook into an alternative ISO 3166 "num-3"
 *       lookup function (provided by the user) the
 *       GS1_LINTER_CUSTOM_ISO3166_LOOKUP_H macro may be set to the name of a
 *       header file to be included that defines a custom
 *       `GS1_LINTER_CUSTOM_ISO3166_LOOKUP` macro.
 * @note If provided, the GS1_LINTER_CUSTOM_ISO3166_LOOKUP macro shall invoke
 *       whatever functionality is available in the user-provided lookup
 *       function, then using the result must assign to a locally-scoped
 *       variable as follows:
 *         - `valid`: Set to 1 if the lookup was successful. Otherwise 0.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_NOT_ISO3166 if the data is not a num-3 country code.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_iso3166(const char *data, size_t *err_pos, size_t *err_len)
{

	/*
	 * Allow for a custom replacement of the lookup code to be provided.
	 *
	 */
#ifdef GS1_LINTER_CUSTOM_ISO3166_LOOKUP
#define GS1_LINTER_ISO3166_LOOKUP(cc) GS1_LINTER_CUSTOM_ISO3166_LOOKUP(cc)
#else

	/*
	 *  Set of ISO 3166 num-3 country codes
	 *
	 *  MAINTENANCE NOTE:
	 *
	 *  Updates to the ISO 3166 num-3 country code list are provided here:
	 *
	 *  https://isotc.iso.org/livelink/livelink?func=ll&objId=16944257&objAction=browse&viewType=1
	 *
	 */
	static const char iso3166[][4] = {
		"004", "008", "010", "012", "016", "020", "024", "028", "031", "032", "036", "040", "044", "048",
		"050", "051", "052", "056", "060", "064", "068", "070", "072", "074", "076", "084", "086", "090", "092", "096",
		"100", "104", "108", "112", "116", "120", "124", "132", "136", "140", "144", "148",
		"152", "156", "158", "162", "166", "170", "174", "175", "178", "180", "184", "188", "191", "192", "196",
		"203", "204", "208", "212", "214", "218", "222", "226", "231", "232", "233", "234", "238", "239", "242", "246", "248",
		"250", "254", "258", "260", "262", "266", "268", "270", "275", "276", "288", "292", "296",
		"300", "304", "308", "312", "316", "320", "324", "328", "332", "334", "336", "340", "344", "348",
		"352", "356", "360", "364", "368", "372", "376", "380", "384", "388", "392", "398",
		"400", "404", "408", "410", "414", "417", "418", "422", "426", "428", "430", "434", "438", "440", "442", "446",
		"450", "454", "458", "462", "466", "470", "474", "478", "480", "484", "492", "496", "498", "499",
		"500", "504", "508", "512", "516", "520", "524", "528", "531", "533", "534", "535", "540", "548",
		"554", "558", "562", "566", "570", "574", "578", "580", "581", "583", "584", "585", "586", "591", "598",
		"600", "604", "608", "612", "616", "620", "624", "626", "630", "634", "638", "642", "643", "646",
		"652", "654", "659", "660", "662", "663", "666", "670", "674", "678", "682", "686", "688", "690", "694",
		"702", "703", "704", "705", "706", "710", "716", "724", "728", "729", "732", "740", "744", "748",
		"752", "756", "760", "762", "764", "768", "772", "776", "780", "784", "788", "792", "795", "796", "798",
		"800", "804", "807", "818", "826", "831", "832", "833", "834", "840",
		"850", "854", "858", "860", "862", "876", "882", "887", "894",
	};

	/*
	 *  Binary search over the above list.
	 *
	 */
/// \cond
#define GS1_LINTER_ISO3166_LOOKUP(cc) do {				\
	size_t s = 0;							\
	size_t e = sizeof(iso3166) / sizeof(iso3166[0]);		\
	while (s < e) {							\
		const size_t m = s + (e - s) / 2;			\
		const int cmp = strcmp(iso3166[m], cc);			\
		if (cmp < 0)						\
			s = m + 1;					\
		else if (cmp > 0)					\
			e = m;						\
		else {							\
			valid = 1;					\
			break;						\
		}							\
	}								\
} while (0)
/// \endcond

#endif

	int valid = 0;

	assert(data);

	/*
	 * Ensure that the data is in the list.
	 *
	 */
	GS1_LINTER_ISO3166_LOOKUP(data);
	if (valid)
		return GS1_LINTER_OK;

	/*
	 * If not valid then indicate an error.
	 *
	 */
	if (err_pos) *err_pos = 0;
	if (err_len) *err_len = strlen(data);
	return GS1_LINTER_NOT_ISO3166;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_iso3166(void)
{

	UNIT_TEST_FAIL(gs1_lint_iso3166, "", GS1_LINTER_NOT_ISO3166, "**");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "0", GS1_LINTER_NOT_ISO3166, "*0*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "00", GS1_LINTER_NOT_ISO3166, "*00*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "000", GS1_LINTER_NOT_ISO3166, "*000*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "0000", GS1_LINTER_NOT_ISO3166, "*0000*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "_894", GS1_LINTER_NOT_ISO3166, "*_894*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "894_", GS1_LINTER_NOT_ISO3166, "*894_*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "999", GS1_LINTER_NOT_ISO3166, "*999*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "AAA", GS1_LINTER_NOT_ISO3166, "*AAA*");

	UNIT_TEST_FAIL(gs1_lint_iso3166, "000", GS1_LINTER_NOT_ISO3166, "*000*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "001", GS1_LINTER_NOT_ISO3166, "*001*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "002", GS1_LINTER_NOT_ISO3166, "*002*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "003", GS1_LINTER_NOT_ISO3166, "*003*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "004");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "005", GS1_LINTER_NOT_ISO3166, "*005*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "006", GS1_LINTER_NOT_ISO3166, "*006*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "007", GS1_LINTER_NOT_ISO3166, "*007*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "008");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "009", GS1_LINTER_NOT_ISO3166, "*009*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "010");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "011", GS1_LINTER_NOT_ISO3166, "*011*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "012");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "013", GS1_LINTER_NOT_ISO3166, "*013*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "014", GS1_LINTER_NOT_ISO3166, "*014*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "015", GS1_LINTER_NOT_ISO3166, "*015*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "016");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "017", GS1_LINTER_NOT_ISO3166, "*017*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "018", GS1_LINTER_NOT_ISO3166, "*018*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "019", GS1_LINTER_NOT_ISO3166, "*019*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "020");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "021", GS1_LINTER_NOT_ISO3166, "*021*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "022", GS1_LINTER_NOT_ISO3166, "*022*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "023", GS1_LINTER_NOT_ISO3166, "*023*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "024");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "025", GS1_LINTER_NOT_ISO3166, "*025*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "026", GS1_LINTER_NOT_ISO3166, "*026*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "027", GS1_LINTER_NOT_ISO3166, "*027*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "028");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "029", GS1_LINTER_NOT_ISO3166, "*029*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "030", GS1_LINTER_NOT_ISO3166, "*030*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "031");
	UNIT_TEST_PASS(gs1_lint_iso3166, "032");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "033", GS1_LINTER_NOT_ISO3166, "*033*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "034", GS1_LINTER_NOT_ISO3166, "*034*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "035", GS1_LINTER_NOT_ISO3166, "*035*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "036");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "037", GS1_LINTER_NOT_ISO3166, "*037*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "038", GS1_LINTER_NOT_ISO3166, "*038*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "039", GS1_LINTER_NOT_ISO3166, "*039*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "040");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "041", GS1_LINTER_NOT_ISO3166, "*041*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "042", GS1_LINTER_NOT_ISO3166, "*042*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "043", GS1_LINTER_NOT_ISO3166, "*043*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "044");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "045", GS1_LINTER_NOT_ISO3166, "*045*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "046", GS1_LINTER_NOT_ISO3166, "*046*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "047", GS1_LINTER_NOT_ISO3166, "*047*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "048");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "049", GS1_LINTER_NOT_ISO3166, "*049*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "050");
	UNIT_TEST_PASS(gs1_lint_iso3166, "051");
	UNIT_TEST_PASS(gs1_lint_iso3166, "052");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "053", GS1_LINTER_NOT_ISO3166, "*053*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "054", GS1_LINTER_NOT_ISO3166, "*054*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "055", GS1_LINTER_NOT_ISO3166, "*055*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "056");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "057", GS1_LINTER_NOT_ISO3166, "*057*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "058", GS1_LINTER_NOT_ISO3166, "*058*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "059", GS1_LINTER_NOT_ISO3166, "*059*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "060");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "061", GS1_LINTER_NOT_ISO3166, "*061*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "062", GS1_LINTER_NOT_ISO3166, "*062*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "063", GS1_LINTER_NOT_ISO3166, "*063*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "064");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "065", GS1_LINTER_NOT_ISO3166, "*065*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "066", GS1_LINTER_NOT_ISO3166, "*066*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "067", GS1_LINTER_NOT_ISO3166, "*067*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "068");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "069", GS1_LINTER_NOT_ISO3166, "*069*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "070");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "071", GS1_LINTER_NOT_ISO3166, "*071*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "072");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "073", GS1_LINTER_NOT_ISO3166, "*073*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "074");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "075", GS1_LINTER_NOT_ISO3166, "*075*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "076");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "077", GS1_LINTER_NOT_ISO3166, "*077*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "078", GS1_LINTER_NOT_ISO3166, "*078*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "079", GS1_LINTER_NOT_ISO3166, "*079*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "080", GS1_LINTER_NOT_ISO3166, "*080*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "081", GS1_LINTER_NOT_ISO3166, "*081*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "082", GS1_LINTER_NOT_ISO3166, "*082*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "083", GS1_LINTER_NOT_ISO3166, "*083*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "084");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "085", GS1_LINTER_NOT_ISO3166, "*085*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "086");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "087", GS1_LINTER_NOT_ISO3166, "*087*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "088", GS1_LINTER_NOT_ISO3166, "*088*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "089", GS1_LINTER_NOT_ISO3166, "*089*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "090");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "091", GS1_LINTER_NOT_ISO3166, "*091*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "092");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "093", GS1_LINTER_NOT_ISO3166, "*093*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "094", GS1_LINTER_NOT_ISO3166, "*094*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "095", GS1_LINTER_NOT_ISO3166, "*095*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "096");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "097", GS1_LINTER_NOT_ISO3166, "*097*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "098", GS1_LINTER_NOT_ISO3166, "*098*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "099", GS1_LINTER_NOT_ISO3166, "*099*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "100");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "101", GS1_LINTER_NOT_ISO3166, "*101*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "102", GS1_LINTER_NOT_ISO3166, "*102*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "103", GS1_LINTER_NOT_ISO3166, "*103*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "104");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "105", GS1_LINTER_NOT_ISO3166, "*105*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "106", GS1_LINTER_NOT_ISO3166, "*106*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "107", GS1_LINTER_NOT_ISO3166, "*107*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "108");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "109", GS1_LINTER_NOT_ISO3166, "*109*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "110", GS1_LINTER_NOT_ISO3166, "*110*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "111", GS1_LINTER_NOT_ISO3166, "*111*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "112");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "113", GS1_LINTER_NOT_ISO3166, "*113*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "114", GS1_LINTER_NOT_ISO3166, "*114*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "115", GS1_LINTER_NOT_ISO3166, "*115*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "116");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "117", GS1_LINTER_NOT_ISO3166, "*117*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "118", GS1_LINTER_NOT_ISO3166, "*118*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "119", GS1_LINTER_NOT_ISO3166, "*119*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "120");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "121", GS1_LINTER_NOT_ISO3166, "*121*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "122", GS1_LINTER_NOT_ISO3166, "*122*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "123", GS1_LINTER_NOT_ISO3166, "*123*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "124");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "125", GS1_LINTER_NOT_ISO3166, "*125*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "126", GS1_LINTER_NOT_ISO3166, "*126*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "127", GS1_LINTER_NOT_ISO3166, "*127*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "128", GS1_LINTER_NOT_ISO3166, "*128*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "129", GS1_LINTER_NOT_ISO3166, "*129*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "130", GS1_LINTER_NOT_ISO3166, "*130*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "131", GS1_LINTER_NOT_ISO3166, "*131*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "132");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "133", GS1_LINTER_NOT_ISO3166, "*133*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "134", GS1_LINTER_NOT_ISO3166, "*134*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "135", GS1_LINTER_NOT_ISO3166, "*135*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "136");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "137", GS1_LINTER_NOT_ISO3166, "*137*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "138", GS1_LINTER_NOT_ISO3166, "*138*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "139", GS1_LINTER_NOT_ISO3166, "*139*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "140");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "141", GS1_LINTER_NOT_ISO3166, "*141*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "142", GS1_LINTER_NOT_ISO3166, "*142*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "143", GS1_LINTER_NOT_ISO3166, "*143*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "144");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "145", GS1_LINTER_NOT_ISO3166, "*145*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "146", GS1_LINTER_NOT_ISO3166, "*146*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "147", GS1_LINTER_NOT_ISO3166, "*147*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "148");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "149", GS1_LINTER_NOT_ISO3166, "*149*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "150", GS1_LINTER_NOT_ISO3166, "*150*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "151", GS1_LINTER_NOT_ISO3166, "*151*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "152");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "153", GS1_LINTER_NOT_ISO3166, "*153*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "154", GS1_LINTER_NOT_ISO3166, "*154*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "155", GS1_LINTER_NOT_ISO3166, "*155*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "156");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "157", GS1_LINTER_NOT_ISO3166, "*157*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "158");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "159", GS1_LINTER_NOT_ISO3166, "*159*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "160", GS1_LINTER_NOT_ISO3166, "*160*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "161", GS1_LINTER_NOT_ISO3166, "*161*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "162");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "163", GS1_LINTER_NOT_ISO3166, "*163*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "164", GS1_LINTER_NOT_ISO3166, "*164*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "165", GS1_LINTER_NOT_ISO3166, "*165*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "166");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "167", GS1_LINTER_NOT_ISO3166, "*167*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "168", GS1_LINTER_NOT_ISO3166, "*168*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "169", GS1_LINTER_NOT_ISO3166, "*169*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "170");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "171", GS1_LINTER_NOT_ISO3166, "*171*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "172", GS1_LINTER_NOT_ISO3166, "*172*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "173", GS1_LINTER_NOT_ISO3166, "*173*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "174");
	UNIT_TEST_PASS(gs1_lint_iso3166, "175");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "176", GS1_LINTER_NOT_ISO3166, "*176*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "177", GS1_LINTER_NOT_ISO3166, "*177*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "178");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "179", GS1_LINTER_NOT_ISO3166, "*179*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "180");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "181", GS1_LINTER_NOT_ISO3166, "*181*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "182", GS1_LINTER_NOT_ISO3166, "*182*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "183", GS1_LINTER_NOT_ISO3166, "*183*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "184");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "185", GS1_LINTER_NOT_ISO3166, "*185*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "186", GS1_LINTER_NOT_ISO3166, "*186*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "187", GS1_LINTER_NOT_ISO3166, "*187*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "188");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "189", GS1_LINTER_NOT_ISO3166, "*189*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "190", GS1_LINTER_NOT_ISO3166, "*190*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "191");
	UNIT_TEST_PASS(gs1_lint_iso3166, "192");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "193", GS1_LINTER_NOT_ISO3166, "*193*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "194", GS1_LINTER_NOT_ISO3166, "*194*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "195", GS1_LINTER_NOT_ISO3166, "*195*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "196");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "197", GS1_LINTER_NOT_ISO3166, "*197*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "198", GS1_LINTER_NOT_ISO3166, "*198*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "199", GS1_LINTER_NOT_ISO3166, "*199*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "200", GS1_LINTER_NOT_ISO3166, "*200*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "201", GS1_LINTER_NOT_ISO3166, "*201*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "202", GS1_LINTER_NOT_ISO3166, "*202*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "203");
	UNIT_TEST_PASS(gs1_lint_iso3166, "204");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "205", GS1_LINTER_NOT_ISO3166, "*205*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "206", GS1_LINTER_NOT_ISO3166, "*206*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "207", GS1_LINTER_NOT_ISO3166, "*207*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "208");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "209", GS1_LINTER_NOT_ISO3166, "*209*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "210", GS1_LINTER_NOT_ISO3166, "*210*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "211", GS1_LINTER_NOT_ISO3166, "*211*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "212");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "213", GS1_LINTER_NOT_ISO3166, "*213*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "214");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "215", GS1_LINTER_NOT_ISO3166, "*215*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "216", GS1_LINTER_NOT_ISO3166, "*216*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "217", GS1_LINTER_NOT_ISO3166, "*217*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "218");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "219", GS1_LINTER_NOT_ISO3166, "*219*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "220", GS1_LINTER_NOT_ISO3166, "*220*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "221", GS1_LINTER_NOT_ISO3166, "*221*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "222");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "223", GS1_LINTER_NOT_ISO3166, "*223*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "224", GS1_LINTER_NOT_ISO3166, "*224*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "225", GS1_LINTER_NOT_ISO3166, "*225*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "226");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "227", GS1_LINTER_NOT_ISO3166, "*227*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "228", GS1_LINTER_NOT_ISO3166, "*228*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "229", GS1_LINTER_NOT_ISO3166, "*229*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "230", GS1_LINTER_NOT_ISO3166, "*230*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "231");
	UNIT_TEST_PASS(gs1_lint_iso3166, "232");
	UNIT_TEST_PASS(gs1_lint_iso3166, "233");
	UNIT_TEST_PASS(gs1_lint_iso3166, "234");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "235", GS1_LINTER_NOT_ISO3166, "*235*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "236", GS1_LINTER_NOT_ISO3166, "*236*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "237", GS1_LINTER_NOT_ISO3166, "*237*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "238");
	UNIT_TEST_PASS(gs1_lint_iso3166, "239");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "240", GS1_LINTER_NOT_ISO3166, "*240*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "241", GS1_LINTER_NOT_ISO3166, "*241*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "242");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "243", GS1_LINTER_NOT_ISO3166, "*243*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "244", GS1_LINTER_NOT_ISO3166, "*244*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "245", GS1_LINTER_NOT_ISO3166, "*245*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "246");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "247", GS1_LINTER_NOT_ISO3166, "*247*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "248");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "249", GS1_LINTER_NOT_ISO3166, "*249*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "250");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "251", GS1_LINTER_NOT_ISO3166, "*251*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "252", GS1_LINTER_NOT_ISO3166, "*252*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "253", GS1_LINTER_NOT_ISO3166, "*253*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "254");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "255", GS1_LINTER_NOT_ISO3166, "*255*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "256", GS1_LINTER_NOT_ISO3166, "*256*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "257", GS1_LINTER_NOT_ISO3166, "*257*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "258");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "259", GS1_LINTER_NOT_ISO3166, "*259*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "260");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "261", GS1_LINTER_NOT_ISO3166, "*261*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "262");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "263", GS1_LINTER_NOT_ISO3166, "*263*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "264", GS1_LINTER_NOT_ISO3166, "*264*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "265", GS1_LINTER_NOT_ISO3166, "*265*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "266");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "267", GS1_LINTER_NOT_ISO3166, "*267*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "268");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "269", GS1_LINTER_NOT_ISO3166, "*269*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "270");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "271", GS1_LINTER_NOT_ISO3166, "*271*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "272", GS1_LINTER_NOT_ISO3166, "*272*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "273", GS1_LINTER_NOT_ISO3166, "*273*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "274", GS1_LINTER_NOT_ISO3166, "*274*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "275");
	UNIT_TEST_PASS(gs1_lint_iso3166, "276");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "277", GS1_LINTER_NOT_ISO3166, "*277*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "278", GS1_LINTER_NOT_ISO3166, "*278*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "279", GS1_LINTER_NOT_ISO3166, "*279*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "280", GS1_LINTER_NOT_ISO3166, "*280*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "281", GS1_LINTER_NOT_ISO3166, "*281*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "282", GS1_LINTER_NOT_ISO3166, "*282*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "283", GS1_LINTER_NOT_ISO3166, "*283*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "284", GS1_LINTER_NOT_ISO3166, "*284*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "285", GS1_LINTER_NOT_ISO3166, "*285*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "286", GS1_LINTER_NOT_ISO3166, "*286*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "287", GS1_LINTER_NOT_ISO3166, "*287*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "288");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "289", GS1_LINTER_NOT_ISO3166, "*289*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "290", GS1_LINTER_NOT_ISO3166, "*290*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "291", GS1_LINTER_NOT_ISO3166, "*291*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "292");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "293", GS1_LINTER_NOT_ISO3166, "*293*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "294", GS1_LINTER_NOT_ISO3166, "*294*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "295", GS1_LINTER_NOT_ISO3166, "*295*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "296");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "297", GS1_LINTER_NOT_ISO3166, "*297*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "298", GS1_LINTER_NOT_ISO3166, "*298*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "299", GS1_LINTER_NOT_ISO3166, "*299*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "300");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "301", GS1_LINTER_NOT_ISO3166, "*301*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "302", GS1_LINTER_NOT_ISO3166, "*302*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "303", GS1_LINTER_NOT_ISO3166, "*303*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "304");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "305", GS1_LINTER_NOT_ISO3166, "*305*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "306", GS1_LINTER_NOT_ISO3166, "*306*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "307", GS1_LINTER_NOT_ISO3166, "*307*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "308");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "309", GS1_LINTER_NOT_ISO3166, "*309*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "310", GS1_LINTER_NOT_ISO3166, "*310*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "311", GS1_LINTER_NOT_ISO3166, "*311*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "312");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "313", GS1_LINTER_NOT_ISO3166, "*313*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "314", GS1_LINTER_NOT_ISO3166, "*314*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "315", GS1_LINTER_NOT_ISO3166, "*315*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "316");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "317", GS1_LINTER_NOT_ISO3166, "*317*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "318", GS1_LINTER_NOT_ISO3166, "*318*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "319", GS1_LINTER_NOT_ISO3166, "*319*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "320");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "321", GS1_LINTER_NOT_ISO3166, "*321*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "322", GS1_LINTER_NOT_ISO3166, "*322*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "323", GS1_LINTER_NOT_ISO3166, "*323*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "324");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "325", GS1_LINTER_NOT_ISO3166, "*325*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "326", GS1_LINTER_NOT_ISO3166, "*326*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "327", GS1_LINTER_NOT_ISO3166, "*327*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "328");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "329", GS1_LINTER_NOT_ISO3166, "*329*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "330", GS1_LINTER_NOT_ISO3166, "*330*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "331", GS1_LINTER_NOT_ISO3166, "*331*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "332");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "333", GS1_LINTER_NOT_ISO3166, "*333*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "334");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "335", GS1_LINTER_NOT_ISO3166, "*335*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "336");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "337", GS1_LINTER_NOT_ISO3166, "*337*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "338", GS1_LINTER_NOT_ISO3166, "*338*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "339", GS1_LINTER_NOT_ISO3166, "*339*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "340");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "341", GS1_LINTER_NOT_ISO3166, "*341*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "342", GS1_LINTER_NOT_ISO3166, "*342*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "343", GS1_LINTER_NOT_ISO3166, "*343*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "344");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "345", GS1_LINTER_NOT_ISO3166, "*345*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "346", GS1_LINTER_NOT_ISO3166, "*346*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "347", GS1_LINTER_NOT_ISO3166, "*347*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "348");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "349", GS1_LINTER_NOT_ISO3166, "*349*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "350", GS1_LINTER_NOT_ISO3166, "*350*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "351", GS1_LINTER_NOT_ISO3166, "*351*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "352");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "353", GS1_LINTER_NOT_ISO3166, "*353*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "354", GS1_LINTER_NOT_ISO3166, "*354*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "355", GS1_LINTER_NOT_ISO3166, "*355*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "356");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "357", GS1_LINTER_NOT_ISO3166, "*357*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "358", GS1_LINTER_NOT_ISO3166, "*358*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "359", GS1_LINTER_NOT_ISO3166, "*359*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "360");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "361", GS1_LINTER_NOT_ISO3166, "*361*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "362", GS1_LINTER_NOT_ISO3166, "*362*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "363", GS1_LINTER_NOT_ISO3166, "*363*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "364");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "365", GS1_LINTER_NOT_ISO3166, "*365*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "366", GS1_LINTER_NOT_ISO3166, "*366*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "367", GS1_LINTER_NOT_ISO3166, "*367*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "368");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "369", GS1_LINTER_NOT_ISO3166, "*369*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "370", GS1_LINTER_NOT_ISO3166, "*370*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "371", GS1_LINTER_NOT_ISO3166, "*371*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "372");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "373", GS1_LINTER_NOT_ISO3166, "*373*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "374", GS1_LINTER_NOT_ISO3166, "*374*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "375", GS1_LINTER_NOT_ISO3166, "*375*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "376");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "377", GS1_LINTER_NOT_ISO3166, "*377*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "378", GS1_LINTER_NOT_ISO3166, "*378*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "379", GS1_LINTER_NOT_ISO3166, "*379*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "380");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "381", GS1_LINTER_NOT_ISO3166, "*381*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "382", GS1_LINTER_NOT_ISO3166, "*382*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "383", GS1_LINTER_NOT_ISO3166, "*383*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "384");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "385", GS1_LINTER_NOT_ISO3166, "*385*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "386", GS1_LINTER_NOT_ISO3166, "*386*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "387", GS1_LINTER_NOT_ISO3166, "*387*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "388");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "389", GS1_LINTER_NOT_ISO3166, "*389*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "390", GS1_LINTER_NOT_ISO3166, "*390*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "391", GS1_LINTER_NOT_ISO3166, "*391*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "392");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "393", GS1_LINTER_NOT_ISO3166, "*393*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "394", GS1_LINTER_NOT_ISO3166, "*394*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "395", GS1_LINTER_NOT_ISO3166, "*395*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "396", GS1_LINTER_NOT_ISO3166, "*396*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "397", GS1_LINTER_NOT_ISO3166, "*397*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "398");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "399", GS1_LINTER_NOT_ISO3166, "*399*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "400");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "401", GS1_LINTER_NOT_ISO3166, "*401*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "402", GS1_LINTER_NOT_ISO3166, "*402*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "403", GS1_LINTER_NOT_ISO3166, "*403*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "404");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "405", GS1_LINTER_NOT_ISO3166, "*405*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "406", GS1_LINTER_NOT_ISO3166, "*406*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "407", GS1_LINTER_NOT_ISO3166, "*407*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "408");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "409", GS1_LINTER_NOT_ISO3166, "*409*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "410");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "411", GS1_LINTER_NOT_ISO3166, "*411*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "412", GS1_LINTER_NOT_ISO3166, "*412*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "413", GS1_LINTER_NOT_ISO3166, "*413*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "414");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "415", GS1_LINTER_NOT_ISO3166, "*415*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "416", GS1_LINTER_NOT_ISO3166, "*416*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "417");
	UNIT_TEST_PASS(gs1_lint_iso3166, "418");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "419", GS1_LINTER_NOT_ISO3166, "*419*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "420", GS1_LINTER_NOT_ISO3166, "*420*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "421", GS1_LINTER_NOT_ISO3166, "*421*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "422");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "423", GS1_LINTER_NOT_ISO3166, "*423*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "424", GS1_LINTER_NOT_ISO3166, "*424*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "425", GS1_LINTER_NOT_ISO3166, "*425*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "426");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "427", GS1_LINTER_NOT_ISO3166, "*427*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "428");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "429", GS1_LINTER_NOT_ISO3166, "*429*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "430");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "431", GS1_LINTER_NOT_ISO3166, "*431*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "432", GS1_LINTER_NOT_ISO3166, "*432*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "433", GS1_LINTER_NOT_ISO3166, "*433*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "434");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "435", GS1_LINTER_NOT_ISO3166, "*435*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "436", GS1_LINTER_NOT_ISO3166, "*436*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "437", GS1_LINTER_NOT_ISO3166, "*437*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "438");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "439", GS1_LINTER_NOT_ISO3166, "*439*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "440");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "441", GS1_LINTER_NOT_ISO3166, "*441*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "442");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "443", GS1_LINTER_NOT_ISO3166, "*443*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "444", GS1_LINTER_NOT_ISO3166, "*444*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "445", GS1_LINTER_NOT_ISO3166, "*445*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "446");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "447", GS1_LINTER_NOT_ISO3166, "*447*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "448", GS1_LINTER_NOT_ISO3166, "*448*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "449", GS1_LINTER_NOT_ISO3166, "*449*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "450");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "451", GS1_LINTER_NOT_ISO3166, "*451*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "452", GS1_LINTER_NOT_ISO3166, "*452*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "453", GS1_LINTER_NOT_ISO3166, "*453*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "454");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "455", GS1_LINTER_NOT_ISO3166, "*455*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "456", GS1_LINTER_NOT_ISO3166, "*456*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "457", GS1_LINTER_NOT_ISO3166, "*457*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "458");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "459", GS1_LINTER_NOT_ISO3166, "*459*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "460", GS1_LINTER_NOT_ISO3166, "*460*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "461", GS1_LINTER_NOT_ISO3166, "*461*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "462");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "463", GS1_LINTER_NOT_ISO3166, "*463*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "464", GS1_LINTER_NOT_ISO3166, "*464*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "465", GS1_LINTER_NOT_ISO3166, "*465*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "466");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "467", GS1_LINTER_NOT_ISO3166, "*467*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "468", GS1_LINTER_NOT_ISO3166, "*468*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "469", GS1_LINTER_NOT_ISO3166, "*469*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "470");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "471", GS1_LINTER_NOT_ISO3166, "*471*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "472", GS1_LINTER_NOT_ISO3166, "*472*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "473", GS1_LINTER_NOT_ISO3166, "*473*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "474");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "475", GS1_LINTER_NOT_ISO3166, "*475*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "476", GS1_LINTER_NOT_ISO3166, "*476*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "477", GS1_LINTER_NOT_ISO3166, "*477*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "478");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "479", GS1_LINTER_NOT_ISO3166, "*479*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "480");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "481", GS1_LINTER_NOT_ISO3166, "*481*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "482", GS1_LINTER_NOT_ISO3166, "*482*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "483", GS1_LINTER_NOT_ISO3166, "*483*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "484");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "485", GS1_LINTER_NOT_ISO3166, "*485*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "486", GS1_LINTER_NOT_ISO3166, "*486*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "487", GS1_LINTER_NOT_ISO3166, "*487*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "488", GS1_LINTER_NOT_ISO3166, "*488*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "489", GS1_LINTER_NOT_ISO3166, "*489*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "490", GS1_LINTER_NOT_ISO3166, "*490*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "491", GS1_LINTER_NOT_ISO3166, "*491*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "492");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "493", GS1_LINTER_NOT_ISO3166, "*493*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "494", GS1_LINTER_NOT_ISO3166, "*494*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "495", GS1_LINTER_NOT_ISO3166, "*495*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "496");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "497", GS1_LINTER_NOT_ISO3166, "*497*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "498");
	UNIT_TEST_PASS(gs1_lint_iso3166, "499");
	UNIT_TEST_PASS(gs1_lint_iso3166, "500");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "501", GS1_LINTER_NOT_ISO3166, "*501*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "502", GS1_LINTER_NOT_ISO3166, "*502*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "503", GS1_LINTER_NOT_ISO3166, "*503*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "504");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "505", GS1_LINTER_NOT_ISO3166, "*505*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "506", GS1_LINTER_NOT_ISO3166, "*506*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "507", GS1_LINTER_NOT_ISO3166, "*507*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "508");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "509", GS1_LINTER_NOT_ISO3166, "*509*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "510", GS1_LINTER_NOT_ISO3166, "*510*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "511", GS1_LINTER_NOT_ISO3166, "*511*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "512");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "513", GS1_LINTER_NOT_ISO3166, "*513*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "514", GS1_LINTER_NOT_ISO3166, "*514*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "515", GS1_LINTER_NOT_ISO3166, "*515*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "516");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "517", GS1_LINTER_NOT_ISO3166, "*517*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "518", GS1_LINTER_NOT_ISO3166, "*518*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "519", GS1_LINTER_NOT_ISO3166, "*519*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "520");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "521", GS1_LINTER_NOT_ISO3166, "*521*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "522", GS1_LINTER_NOT_ISO3166, "*522*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "523", GS1_LINTER_NOT_ISO3166, "*523*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "524");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "525", GS1_LINTER_NOT_ISO3166, "*525*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "526", GS1_LINTER_NOT_ISO3166, "*526*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "527", GS1_LINTER_NOT_ISO3166, "*527*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "528");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "529", GS1_LINTER_NOT_ISO3166, "*529*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "530", GS1_LINTER_NOT_ISO3166, "*530*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "531");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "532", GS1_LINTER_NOT_ISO3166, "*532*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "533");
	UNIT_TEST_PASS(gs1_lint_iso3166, "534");
	UNIT_TEST_PASS(gs1_lint_iso3166, "535");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "536", GS1_LINTER_NOT_ISO3166, "*536*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "537", GS1_LINTER_NOT_ISO3166, "*537*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "538", GS1_LINTER_NOT_ISO3166, "*538*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "539", GS1_LINTER_NOT_ISO3166, "*539*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "540");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "541", GS1_LINTER_NOT_ISO3166, "*541*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "542", GS1_LINTER_NOT_ISO3166, "*542*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "543", GS1_LINTER_NOT_ISO3166, "*543*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "544", GS1_LINTER_NOT_ISO3166, "*544*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "545", GS1_LINTER_NOT_ISO3166, "*545*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "546", GS1_LINTER_NOT_ISO3166, "*546*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "547", GS1_LINTER_NOT_ISO3166, "*547*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "548");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "549", GS1_LINTER_NOT_ISO3166, "*549*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "550", GS1_LINTER_NOT_ISO3166, "*550*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "551", GS1_LINTER_NOT_ISO3166, "*551*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "552", GS1_LINTER_NOT_ISO3166, "*552*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "553", GS1_LINTER_NOT_ISO3166, "*553*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "554");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "555", GS1_LINTER_NOT_ISO3166, "*555*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "556", GS1_LINTER_NOT_ISO3166, "*556*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "557", GS1_LINTER_NOT_ISO3166, "*557*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "558");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "559", GS1_LINTER_NOT_ISO3166, "*559*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "560", GS1_LINTER_NOT_ISO3166, "*560*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "561", GS1_LINTER_NOT_ISO3166, "*561*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "562");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "563", GS1_LINTER_NOT_ISO3166, "*563*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "564", GS1_LINTER_NOT_ISO3166, "*564*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "565", GS1_LINTER_NOT_ISO3166, "*565*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "566");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "567", GS1_LINTER_NOT_ISO3166, "*567*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "568", GS1_LINTER_NOT_ISO3166, "*568*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "569", GS1_LINTER_NOT_ISO3166, "*569*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "570");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "571", GS1_LINTER_NOT_ISO3166, "*571*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "572", GS1_LINTER_NOT_ISO3166, "*572*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "573", GS1_LINTER_NOT_ISO3166, "*573*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "574");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "575", GS1_LINTER_NOT_ISO3166, "*575*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "576", GS1_LINTER_NOT_ISO3166, "*576*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "577", GS1_LINTER_NOT_ISO3166, "*577*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "578");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "579", GS1_LINTER_NOT_ISO3166, "*579*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "580");
	UNIT_TEST_PASS(gs1_lint_iso3166, "581");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "582", GS1_LINTER_NOT_ISO3166, "*582*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "583");
	UNIT_TEST_PASS(gs1_lint_iso3166, "584");
	UNIT_TEST_PASS(gs1_lint_iso3166, "585");
	UNIT_TEST_PASS(gs1_lint_iso3166, "586");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "587", GS1_LINTER_NOT_ISO3166, "*587*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "588", GS1_LINTER_NOT_ISO3166, "*588*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "589", GS1_LINTER_NOT_ISO3166, "*589*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "590", GS1_LINTER_NOT_ISO3166, "*590*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "591");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "592", GS1_LINTER_NOT_ISO3166, "*592*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "593", GS1_LINTER_NOT_ISO3166, "*593*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "594", GS1_LINTER_NOT_ISO3166, "*594*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "595", GS1_LINTER_NOT_ISO3166, "*595*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "596", GS1_LINTER_NOT_ISO3166, "*596*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "597", GS1_LINTER_NOT_ISO3166, "*597*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "598");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "599", GS1_LINTER_NOT_ISO3166, "*599*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "600");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "601", GS1_LINTER_NOT_ISO3166, "*601*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "602", GS1_LINTER_NOT_ISO3166, "*602*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "603", GS1_LINTER_NOT_ISO3166, "*603*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "604");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "605", GS1_LINTER_NOT_ISO3166, "*605*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "606", GS1_LINTER_NOT_ISO3166, "*606*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "607", GS1_LINTER_NOT_ISO3166, "*607*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "608");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "609", GS1_LINTER_NOT_ISO3166, "*609*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "610", GS1_LINTER_NOT_ISO3166, "*610*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "611", GS1_LINTER_NOT_ISO3166, "*611*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "612");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "613", GS1_LINTER_NOT_ISO3166, "*613*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "614", GS1_LINTER_NOT_ISO3166, "*614*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "615", GS1_LINTER_NOT_ISO3166, "*615*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "616");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "617", GS1_LINTER_NOT_ISO3166, "*617*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "618", GS1_LINTER_NOT_ISO3166, "*618*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "619", GS1_LINTER_NOT_ISO3166, "*619*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "620");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "621", GS1_LINTER_NOT_ISO3166, "*621*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "622", GS1_LINTER_NOT_ISO3166, "*622*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "623", GS1_LINTER_NOT_ISO3166, "*623*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "624");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "625", GS1_LINTER_NOT_ISO3166, "*625*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "626");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "627", GS1_LINTER_NOT_ISO3166, "*627*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "628", GS1_LINTER_NOT_ISO3166, "*628*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "629", GS1_LINTER_NOT_ISO3166, "*629*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "630");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "631", GS1_LINTER_NOT_ISO3166, "*631*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "632", GS1_LINTER_NOT_ISO3166, "*632*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "633", GS1_LINTER_NOT_ISO3166, "*633*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "634");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "635", GS1_LINTER_NOT_ISO3166, "*635*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "636", GS1_LINTER_NOT_ISO3166, "*636*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "637", GS1_LINTER_NOT_ISO3166, "*637*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "638");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "639", GS1_LINTER_NOT_ISO3166, "*639*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "640", GS1_LINTER_NOT_ISO3166, "*640*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "641", GS1_LINTER_NOT_ISO3166, "*641*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "642");
	UNIT_TEST_PASS(gs1_lint_iso3166, "643");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "644", GS1_LINTER_NOT_ISO3166, "*644*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "645", GS1_LINTER_NOT_ISO3166, "*645*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "646");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "647", GS1_LINTER_NOT_ISO3166, "*647*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "648", GS1_LINTER_NOT_ISO3166, "*648*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "649", GS1_LINTER_NOT_ISO3166, "*649*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "650", GS1_LINTER_NOT_ISO3166, "*650*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "651", GS1_LINTER_NOT_ISO3166, "*651*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "652");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "653", GS1_LINTER_NOT_ISO3166, "*653*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "654");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "655", GS1_LINTER_NOT_ISO3166, "*655*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "656", GS1_LINTER_NOT_ISO3166, "*656*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "657", GS1_LINTER_NOT_ISO3166, "*657*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "658", GS1_LINTER_NOT_ISO3166, "*658*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "659");
	UNIT_TEST_PASS(gs1_lint_iso3166, "660");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "661", GS1_LINTER_NOT_ISO3166, "*661*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "662");
	UNIT_TEST_PASS(gs1_lint_iso3166, "663");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "664", GS1_LINTER_NOT_ISO3166, "*664*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "665", GS1_LINTER_NOT_ISO3166, "*665*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "666");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "667", GS1_LINTER_NOT_ISO3166, "*667*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "668", GS1_LINTER_NOT_ISO3166, "*668*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "669", GS1_LINTER_NOT_ISO3166, "*669*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "670");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "671", GS1_LINTER_NOT_ISO3166, "*671*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "672", GS1_LINTER_NOT_ISO3166, "*672*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "673", GS1_LINTER_NOT_ISO3166, "*673*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "674");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "675", GS1_LINTER_NOT_ISO3166, "*675*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "676", GS1_LINTER_NOT_ISO3166, "*676*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "677", GS1_LINTER_NOT_ISO3166, "*677*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "678");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "679", GS1_LINTER_NOT_ISO3166, "*679*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "680", GS1_LINTER_NOT_ISO3166, "*680*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "681", GS1_LINTER_NOT_ISO3166, "*681*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "682");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "683", GS1_LINTER_NOT_ISO3166, "*683*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "684", GS1_LINTER_NOT_ISO3166, "*684*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "685", GS1_LINTER_NOT_ISO3166, "*685*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "686");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "687", GS1_LINTER_NOT_ISO3166, "*687*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "688");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "689", GS1_LINTER_NOT_ISO3166, "*689*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "690");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "691", GS1_LINTER_NOT_ISO3166, "*691*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "692", GS1_LINTER_NOT_ISO3166, "*692*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "693", GS1_LINTER_NOT_ISO3166, "*693*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "694");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "695", GS1_LINTER_NOT_ISO3166, "*695*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "696", GS1_LINTER_NOT_ISO3166, "*696*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "697", GS1_LINTER_NOT_ISO3166, "*697*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "698", GS1_LINTER_NOT_ISO3166, "*698*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "699", GS1_LINTER_NOT_ISO3166, "*699*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "700", GS1_LINTER_NOT_ISO3166, "*700*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "701", GS1_LINTER_NOT_ISO3166, "*701*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "702");
	UNIT_TEST_PASS(gs1_lint_iso3166, "703");
	UNIT_TEST_PASS(gs1_lint_iso3166, "704");
	UNIT_TEST_PASS(gs1_lint_iso3166, "705");
	UNIT_TEST_PASS(gs1_lint_iso3166, "706");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "707", GS1_LINTER_NOT_ISO3166, "*707*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "708", GS1_LINTER_NOT_ISO3166, "*708*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "709", GS1_LINTER_NOT_ISO3166, "*709*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "710");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "711", GS1_LINTER_NOT_ISO3166, "*711*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "712", GS1_LINTER_NOT_ISO3166, "*712*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "713", GS1_LINTER_NOT_ISO3166, "*713*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "714", GS1_LINTER_NOT_ISO3166, "*714*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "715", GS1_LINTER_NOT_ISO3166, "*715*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "716");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "717", GS1_LINTER_NOT_ISO3166, "*717*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "718", GS1_LINTER_NOT_ISO3166, "*718*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "719", GS1_LINTER_NOT_ISO3166, "*719*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "720", GS1_LINTER_NOT_ISO3166, "*720*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "721", GS1_LINTER_NOT_ISO3166, "*721*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "722", GS1_LINTER_NOT_ISO3166, "*722*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "723", GS1_LINTER_NOT_ISO3166, "*723*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "724");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "725", GS1_LINTER_NOT_ISO3166, "*725*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "726", GS1_LINTER_NOT_ISO3166, "*726*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "727", GS1_LINTER_NOT_ISO3166, "*727*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "728");
	UNIT_TEST_PASS(gs1_lint_iso3166, "729");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "730", GS1_LINTER_NOT_ISO3166, "*730*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "731", GS1_LINTER_NOT_ISO3166, "*731*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "732");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "733", GS1_LINTER_NOT_ISO3166, "*733*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "734", GS1_LINTER_NOT_ISO3166, "*734*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "735", GS1_LINTER_NOT_ISO3166, "*735*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "736", GS1_LINTER_NOT_ISO3166, "*736*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "737", GS1_LINTER_NOT_ISO3166, "*737*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "738", GS1_LINTER_NOT_ISO3166, "*738*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "739", GS1_LINTER_NOT_ISO3166, "*739*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "740");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "741", GS1_LINTER_NOT_ISO3166, "*741*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "742", GS1_LINTER_NOT_ISO3166, "*742*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "743", GS1_LINTER_NOT_ISO3166, "*743*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "744");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "745", GS1_LINTER_NOT_ISO3166, "*745*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "746", GS1_LINTER_NOT_ISO3166, "*746*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "747", GS1_LINTER_NOT_ISO3166, "*747*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "748");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "749", GS1_LINTER_NOT_ISO3166, "*749*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "750", GS1_LINTER_NOT_ISO3166, "*750*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "751", GS1_LINTER_NOT_ISO3166, "*751*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "752");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "753", GS1_LINTER_NOT_ISO3166, "*753*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "754", GS1_LINTER_NOT_ISO3166, "*754*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "755", GS1_LINTER_NOT_ISO3166, "*755*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "756");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "757", GS1_LINTER_NOT_ISO3166, "*757*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "758", GS1_LINTER_NOT_ISO3166, "*758*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "759", GS1_LINTER_NOT_ISO3166, "*759*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "760");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "761", GS1_LINTER_NOT_ISO3166, "*761*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "762");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "763", GS1_LINTER_NOT_ISO3166, "*763*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "764");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "765", GS1_LINTER_NOT_ISO3166, "*765*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "766", GS1_LINTER_NOT_ISO3166, "*766*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "767", GS1_LINTER_NOT_ISO3166, "*767*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "768");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "769", GS1_LINTER_NOT_ISO3166, "*769*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "770", GS1_LINTER_NOT_ISO3166, "*770*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "771", GS1_LINTER_NOT_ISO3166, "*771*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "772");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "773", GS1_LINTER_NOT_ISO3166, "*773*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "774", GS1_LINTER_NOT_ISO3166, "*774*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "775", GS1_LINTER_NOT_ISO3166, "*775*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "776");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "777", GS1_LINTER_NOT_ISO3166, "*777*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "778", GS1_LINTER_NOT_ISO3166, "*778*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "779", GS1_LINTER_NOT_ISO3166, "*779*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "780");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "781", GS1_LINTER_NOT_ISO3166, "*781*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "782", GS1_LINTER_NOT_ISO3166, "*782*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "783", GS1_LINTER_NOT_ISO3166, "*783*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "784");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "785", GS1_LINTER_NOT_ISO3166, "*785*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "786", GS1_LINTER_NOT_ISO3166, "*786*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "787", GS1_LINTER_NOT_ISO3166, "*787*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "788");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "789", GS1_LINTER_NOT_ISO3166, "*789*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "790", GS1_LINTER_NOT_ISO3166, "*790*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "791", GS1_LINTER_NOT_ISO3166, "*791*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "792");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "793", GS1_LINTER_NOT_ISO3166, "*793*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "794", GS1_LINTER_NOT_ISO3166, "*794*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "795");
	UNIT_TEST_PASS(gs1_lint_iso3166, "796");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "797", GS1_LINTER_NOT_ISO3166, "*797*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "798");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "799", GS1_LINTER_NOT_ISO3166, "*799*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "800");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "801", GS1_LINTER_NOT_ISO3166, "*801*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "802", GS1_LINTER_NOT_ISO3166, "*802*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "803", GS1_LINTER_NOT_ISO3166, "*803*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "804");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "805", GS1_LINTER_NOT_ISO3166, "*805*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "806", GS1_LINTER_NOT_ISO3166, "*806*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "807");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "808", GS1_LINTER_NOT_ISO3166, "*808*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "809", GS1_LINTER_NOT_ISO3166, "*809*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "810", GS1_LINTER_NOT_ISO3166, "*810*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "811", GS1_LINTER_NOT_ISO3166, "*811*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "812", GS1_LINTER_NOT_ISO3166, "*812*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "813", GS1_LINTER_NOT_ISO3166, "*813*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "814", GS1_LINTER_NOT_ISO3166, "*814*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "815", GS1_LINTER_NOT_ISO3166, "*815*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "816", GS1_LINTER_NOT_ISO3166, "*816*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "817", GS1_LINTER_NOT_ISO3166, "*817*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "818");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "819", GS1_LINTER_NOT_ISO3166, "*819*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "820", GS1_LINTER_NOT_ISO3166, "*820*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "821", GS1_LINTER_NOT_ISO3166, "*821*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "822", GS1_LINTER_NOT_ISO3166, "*822*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "823", GS1_LINTER_NOT_ISO3166, "*823*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "824", GS1_LINTER_NOT_ISO3166, "*824*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "825", GS1_LINTER_NOT_ISO3166, "*825*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "826");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "827", GS1_LINTER_NOT_ISO3166, "*827*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "828", GS1_LINTER_NOT_ISO3166, "*828*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "829", GS1_LINTER_NOT_ISO3166, "*829*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "830", GS1_LINTER_NOT_ISO3166, "*830*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "831");
	UNIT_TEST_PASS(gs1_lint_iso3166, "832");
	UNIT_TEST_PASS(gs1_lint_iso3166, "833");
	UNIT_TEST_PASS(gs1_lint_iso3166, "834");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "835", GS1_LINTER_NOT_ISO3166, "*835*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "836", GS1_LINTER_NOT_ISO3166, "*836*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "837", GS1_LINTER_NOT_ISO3166, "*837*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "838", GS1_LINTER_NOT_ISO3166, "*838*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "839", GS1_LINTER_NOT_ISO3166, "*839*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "840");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "841", GS1_LINTER_NOT_ISO3166, "*841*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "842", GS1_LINTER_NOT_ISO3166, "*842*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "843", GS1_LINTER_NOT_ISO3166, "*843*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "844", GS1_LINTER_NOT_ISO3166, "*844*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "845", GS1_LINTER_NOT_ISO3166, "*845*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "846", GS1_LINTER_NOT_ISO3166, "*846*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "847", GS1_LINTER_NOT_ISO3166, "*847*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "848", GS1_LINTER_NOT_ISO3166, "*848*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "849", GS1_LINTER_NOT_ISO3166, "*849*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "850");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "851", GS1_LINTER_NOT_ISO3166, "*851*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "852", GS1_LINTER_NOT_ISO3166, "*852*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "853", GS1_LINTER_NOT_ISO3166, "*853*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "854");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "855", GS1_LINTER_NOT_ISO3166, "*855*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "856", GS1_LINTER_NOT_ISO3166, "*856*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "857", GS1_LINTER_NOT_ISO3166, "*857*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "858");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "859", GS1_LINTER_NOT_ISO3166, "*859*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "860");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "861", GS1_LINTER_NOT_ISO3166, "*861*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "862");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "863", GS1_LINTER_NOT_ISO3166, "*863*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "864", GS1_LINTER_NOT_ISO3166, "*864*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "865", GS1_LINTER_NOT_ISO3166, "*865*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "866", GS1_LINTER_NOT_ISO3166, "*866*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "867", GS1_LINTER_NOT_ISO3166, "*867*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "868", GS1_LINTER_NOT_ISO3166, "*868*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "869", GS1_LINTER_NOT_ISO3166, "*869*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "870", GS1_LINTER_NOT_ISO3166, "*870*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "871", GS1_LINTER_NOT_ISO3166, "*871*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "872", GS1_LINTER_NOT_ISO3166, "*872*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "873", GS1_LINTER_NOT_ISO3166, "*873*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "874", GS1_LINTER_NOT_ISO3166, "*874*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "875", GS1_LINTER_NOT_ISO3166, "*875*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "876");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "877", GS1_LINTER_NOT_ISO3166, "*877*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "878", GS1_LINTER_NOT_ISO3166, "*878*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "879", GS1_LINTER_NOT_ISO3166, "*879*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "880", GS1_LINTER_NOT_ISO3166, "*880*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "881", GS1_LINTER_NOT_ISO3166, "*881*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "882");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "883", GS1_LINTER_NOT_ISO3166, "*883*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "884", GS1_LINTER_NOT_ISO3166, "*884*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "885", GS1_LINTER_NOT_ISO3166, "*885*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "886", GS1_LINTER_NOT_ISO3166, "*886*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "887");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "888", GS1_LINTER_NOT_ISO3166, "*888*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "889", GS1_LINTER_NOT_ISO3166, "*889*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "890", GS1_LINTER_NOT_ISO3166, "*890*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "891", GS1_LINTER_NOT_ISO3166, "*891*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "892", GS1_LINTER_NOT_ISO3166, "*892*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "893", GS1_LINTER_NOT_ISO3166, "*893*");
	UNIT_TEST_PASS(gs1_lint_iso3166, "894");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "895", GS1_LINTER_NOT_ISO3166, "*895*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "896", GS1_LINTER_NOT_ISO3166, "*896*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "897", GS1_LINTER_NOT_ISO3166, "*897*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "898", GS1_LINTER_NOT_ISO3166, "*898*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "899", GS1_LINTER_NOT_ISO3166, "*899*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "900", GS1_LINTER_NOT_ISO3166, "*900*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "901", GS1_LINTER_NOT_ISO3166, "*901*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "902", GS1_LINTER_NOT_ISO3166, "*902*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "903", GS1_LINTER_NOT_ISO3166, "*903*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "904", GS1_LINTER_NOT_ISO3166, "*904*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "905", GS1_LINTER_NOT_ISO3166, "*905*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "906", GS1_LINTER_NOT_ISO3166, "*906*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "907", GS1_LINTER_NOT_ISO3166, "*907*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "908", GS1_LINTER_NOT_ISO3166, "*908*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "909", GS1_LINTER_NOT_ISO3166, "*909*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "910", GS1_LINTER_NOT_ISO3166, "*910*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "911", GS1_LINTER_NOT_ISO3166, "*911*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "912", GS1_LINTER_NOT_ISO3166, "*912*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "913", GS1_LINTER_NOT_ISO3166, "*913*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "914", GS1_LINTER_NOT_ISO3166, "*914*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "915", GS1_LINTER_NOT_ISO3166, "*915*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "916", GS1_LINTER_NOT_ISO3166, "*916*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "917", GS1_LINTER_NOT_ISO3166, "*917*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "918", GS1_LINTER_NOT_ISO3166, "*918*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "919", GS1_LINTER_NOT_ISO3166, "*919*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "920", GS1_LINTER_NOT_ISO3166, "*920*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "921", GS1_LINTER_NOT_ISO3166, "*921*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "922", GS1_LINTER_NOT_ISO3166, "*922*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "923", GS1_LINTER_NOT_ISO3166, "*923*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "924", GS1_LINTER_NOT_ISO3166, "*924*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "925", GS1_LINTER_NOT_ISO3166, "*925*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "926", GS1_LINTER_NOT_ISO3166, "*926*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "927", GS1_LINTER_NOT_ISO3166, "*927*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "928", GS1_LINTER_NOT_ISO3166, "*928*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "929", GS1_LINTER_NOT_ISO3166, "*929*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "930", GS1_LINTER_NOT_ISO3166, "*930*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "931", GS1_LINTER_NOT_ISO3166, "*931*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "932", GS1_LINTER_NOT_ISO3166, "*932*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "933", GS1_LINTER_NOT_ISO3166, "*933*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "934", GS1_LINTER_NOT_ISO3166, "*934*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "935", GS1_LINTER_NOT_ISO3166, "*935*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "936", GS1_LINTER_NOT_ISO3166, "*936*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "937", GS1_LINTER_NOT_ISO3166, "*937*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "938", GS1_LINTER_NOT_ISO3166, "*938*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "939", GS1_LINTER_NOT_ISO3166, "*939*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "940", GS1_LINTER_NOT_ISO3166, "*940*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "941", GS1_LINTER_NOT_ISO3166, "*941*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "942", GS1_LINTER_NOT_ISO3166, "*942*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "943", GS1_LINTER_NOT_ISO3166, "*943*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "944", GS1_LINTER_NOT_ISO3166, "*944*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "945", GS1_LINTER_NOT_ISO3166, "*945*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "946", GS1_LINTER_NOT_ISO3166, "*946*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "947", GS1_LINTER_NOT_ISO3166, "*947*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "948", GS1_LINTER_NOT_ISO3166, "*948*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "949", GS1_LINTER_NOT_ISO3166, "*949*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "950", GS1_LINTER_NOT_ISO3166, "*950*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "951", GS1_LINTER_NOT_ISO3166, "*951*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "952", GS1_LINTER_NOT_ISO3166, "*952*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "953", GS1_LINTER_NOT_ISO3166, "*953*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "954", GS1_LINTER_NOT_ISO3166, "*954*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "955", GS1_LINTER_NOT_ISO3166, "*955*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "956", GS1_LINTER_NOT_ISO3166, "*956*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "957", GS1_LINTER_NOT_ISO3166, "*957*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "958", GS1_LINTER_NOT_ISO3166, "*958*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "959", GS1_LINTER_NOT_ISO3166, "*959*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "960", GS1_LINTER_NOT_ISO3166, "*960*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "961", GS1_LINTER_NOT_ISO3166, "*961*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "962", GS1_LINTER_NOT_ISO3166, "*962*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "963", GS1_LINTER_NOT_ISO3166, "*963*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "964", GS1_LINTER_NOT_ISO3166, "*964*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "965", GS1_LINTER_NOT_ISO3166, "*965*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "966", GS1_LINTER_NOT_ISO3166, "*966*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "967", GS1_LINTER_NOT_ISO3166, "*967*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "968", GS1_LINTER_NOT_ISO3166, "*968*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "969", GS1_LINTER_NOT_ISO3166, "*969*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "970", GS1_LINTER_NOT_ISO3166, "*970*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "971", GS1_LINTER_NOT_ISO3166, "*971*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "972", GS1_LINTER_NOT_ISO3166, "*972*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "973", GS1_LINTER_NOT_ISO3166, "*973*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "974", GS1_LINTER_NOT_ISO3166, "*974*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "975", GS1_LINTER_NOT_ISO3166, "*975*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "976", GS1_LINTER_NOT_ISO3166, "*976*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "977", GS1_LINTER_NOT_ISO3166, "*977*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "978", GS1_LINTER_NOT_ISO3166, "*978*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "979", GS1_LINTER_NOT_ISO3166, "*979*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "980", GS1_LINTER_NOT_ISO3166, "*980*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "981", GS1_LINTER_NOT_ISO3166, "*981*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "982", GS1_LINTER_NOT_ISO3166, "*982*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "983", GS1_LINTER_NOT_ISO3166, "*983*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "984", GS1_LINTER_NOT_ISO3166, "*984*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "985", GS1_LINTER_NOT_ISO3166, "*985*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "986", GS1_LINTER_NOT_ISO3166, "*986*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "987", GS1_LINTER_NOT_ISO3166, "*987*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "988", GS1_LINTER_NOT_ISO3166, "*988*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "989", GS1_LINTER_NOT_ISO3166, "*989*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "990", GS1_LINTER_NOT_ISO3166, "*990*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "991", GS1_LINTER_NOT_ISO3166, "*991*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "992", GS1_LINTER_NOT_ISO3166, "*992*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "993", GS1_LINTER_NOT_ISO3166, "*993*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "994", GS1_LINTER_NOT_ISO3166, "*994*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "995", GS1_LINTER_NOT_ISO3166, "*995*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "996", GS1_LINTER_NOT_ISO3166, "*996*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "997", GS1_LINTER_NOT_ISO3166, "*997*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "998", GS1_LINTER_NOT_ISO3166, "*998*");
	UNIT_TEST_FAIL(gs1_lint_iso3166, "999", GS1_LINTER_NOT_ISO3166, "*999*");

}

#endif  /* UNIT_TESTS */