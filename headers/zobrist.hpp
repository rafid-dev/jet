#pragma once

#include "square.hpp"
#include "types.hpp"

#include <array>
#include <random>

namespace chess {

    // Polyglot zobrist hashing
    class zobrist {
    public:
        using Hash = uint64_t;

    private:
        static constexpr inline std::array<Hash, 781> RANDOM_ARRAY = {
            0x786c07ebc92d9e0aULL, 0x22a5b19bc1249959ULL, 0x43d6edc990ede161ULL, 0xee125f4c491f32ebULL, 0x56a0ff269f33ee2fULL, 0xac97dfb6a8970a00ULL, 0x6c32f6b929970716ULL, 0xe627d252f5ef53f0ULL,
            0x46b52ed56a45ad46ULL, 0xadac25b39c9dafe2ULL, 0x486eaf40534af323ULL, 0xab924e1a36f0a284ULL, 0xb6edd2d165ffc0c2ULL, 0x0eecbc788d8bcb14ULL, 0xfd9026749c4965adULL, 0x3031a47b36942cb7ULL,
            0x12c8bb1113b2c304ULL, 0xb00445c25cc77a72ULL, 0xebc7ee7d2e1c6992ULL, 0x841b0af2334e22b7ULL, 0x3224e208456ad52dULL, 0x243f3ba406b68c94ULL, 0xe70c815a28c635e2ULL, 0x607d4ffa4a999fa7ULL,
            0xd90f83187a0a9bc5ULL, 0xa34a573b95881511ULL, 0x82064d6c4430ad2eULL, 0xb3d129fb96438730ULL, 0x91fd982bfdce9ba8ULL, 0x7be3c22db3d85d57ULL, 0x631b593a75273f10ULL, 0x960d7de8efd2683cULL,
            0xb207ee2502c1fe3fULL, 0x99c421efb22db310ULL, 0x97bea45e48653787ULL, 0x7c7ca8c2fdd7b91aULL, 0xb970e6472b3c5718ULL, 0x5133ab902be54b28ULL, 0xcc06697b79106aa5ULL, 0x79aecffa517716eeULL,
            0x51419cf78eeaf48fULL, 0x7e1b32a0d4e22e79ULL, 0x41f08e372e5d8599ULL, 0x650d604578b36900ULL, 0xdfcef361dd92ec53ULL, 0x16915364c4d5514bULL, 0xf58358365087e9feULL, 0xfec68cf50e09558dULL,
            0x89a42e7c0531ec54ULL, 0x55544b09e2b31989ULL, 0xe639c63511dd32feULL, 0xf3bde04b1e03138eULL, 0x86aac16951084b84ULL, 0x4237b9cc3469348eULL, 0x1adc3cb8fa594ea4ULL, 0x4e9105b404591fd1ULL,
            0x80555aea73aed0e8ULL, 0x4b2eaa2174c01d73ULL, 0xc27b94e9fd25487cULL, 0x9f6f6e61f1b69b70ULL, 0x9b28da886a661ad0ULL, 0xa9b8b4fffd8f8792ULL, 0xc97d79ad29eb9b86ULL, 0x19e31ab2ea540730ULL,
            0x4963626c5d82e631ULL, 0x2e39b83069697586ULL, 0xb64486ffa98a3cc1ULL, 0x72393a23c0831245ULL, 0xb3ba33c26b2a18a2ULL, 0x8868fb23ab77ef8bULL, 0x307a4784670563caULL, 0x1b9fa92a5a1da5fbULL,
            0x801b19714b98550aULL, 0x8b70c4b343dc90b4ULL, 0xe13ededdc08566deULL, 0xbacdb689ba610bf3ULL, 0x9cc7975c70342667ULL, 0x4769151ac2e9a3c7ULL, 0xb912e49d5cd38364ULL, 0x4a866a1f921db982ULL,
            0x11f39d238bab333eULL, 0x8bac2a1df1b3f23dULL, 0x0a27ce7b4c31ffdeULL, 0xd47bdac98d62ed50ULL, 0xa139aa5a6a39a4c6ULL, 0xb3d301566b933ecdULL, 0xe684ef7525f02055ULL, 0x614f4c18f902af6bULL,
            0x770cf23b4f86a8b2ULL, 0x9fbb13f86a3115a2ULL, 0x45f9da0fd7ac6eeeULL, 0x763800d5d7d48308ULL, 0x36bd4e5e3a32f852ULL, 0x4c1ed2658657f5edULL, 0xbed62485e47fc7abULL, 0xf260aa349bb103a0ULL,
            0x7b5a61e10df2aae5ULL, 0xb5b8ccc185f5a75dULL, 0xd1b5095ac0640f28ULL, 0x388af5488ae69c10ULL, 0x723b4f9b0564d9f1ULL, 0x0b1b5d0e96801ee8ULL, 0xd595cdda1b867083ULL, 0xa3ed7071fd44c342ULL,
            0xa5eae8691d1d8c70ULL, 0x64e543f9e02a4a52ULL, 0xf80cb66853cfc7afULL, 0x699f567a8360d4c2ULL, 0x30da3e80321e053aULL, 0x54bddce28712e792ULL, 0x22d868ff861c2068ULL, 0xd6f3c2d4570f9de3ULL,
            0x27e047f6bf344035ULL, 0xba3d987df373cd6cULL, 0xba71693250646b35ULL, 0xe0488c6f77b9cc4bULL, 0xf931275ba30a9877ULL, 0x0d03866fd61c20ebULL, 0x84e823168dc4619aULL, 0x57e061b5e68d5c76ULL,
            0x15935d3d04dd82f0ULL, 0x9b2166f214a51827ULL, 0x3f1a40e36b654da0ULL, 0xd5fae0155977548bULL, 0x972b1e6ac2d12172ULL, 0xa970a49e281b5f1dULL, 0x11e9927c1a9d4155ULL, 0x93b2780868967457ULL,
            0xdae2788218b56c18ULL, 0x5e3edf3ad283a9dbULL, 0xdc1b2e6c9d6899b3ULL, 0x14118996f26a42c8ULL, 0xfbb7de5249ffe9fdULL, 0x5d736b45d736b32cULL, 0x97ec80c78fd31b87ULL, 0xeda740425eefcfa9ULL,
            0xe92788f71f5d9247ULL, 0x5a07d456bd9575adULL, 0xb44f97dec80d712cULL, 0xdaac56d6b0baaae1ULL, 0xeefc726f1ea63ce7ULL, 0xc2602a5edab4251cULL, 0x01c827fec1caecf9ULL, 0xf6fec0a6e11aaa29ULL,
            0xebd67d66886bb22fULL, 0x730acb2cc0b17d08ULL, 0x82a2ff235416b851ULL, 0x4d12cd4fc67a7cf0ULL, 0x734f2c1e8891665cULL, 0x0ef2126889ede1c3ULL, 0xe4a26bf6d7d3e28aULL, 0x0968c334ff2b0b48ULL,
            0x3f6686a0999a1b2bULL, 0x2e9224bb889a3cacULL, 0x774051dff7264079ULL, 0xd552e42af4ff600aULL, 0x4fe3d8af68705006ULL, 0xd1469307c9e3f509ULL, 0x66782308f758345cULL, 0x64319bf88d05c20eULL,
            0xc173c7c20b95e7f5ULL, 0xbdab59cfce349d49ULL, 0x8584000574da017cULL, 0x8242a5ab1fd6806dULL, 0x3ece97a0cc7dee05ULL, 0x64cf3136a34b6a7aULL, 0x9437099f8d0cb9d7ULL, 0x7dfdb2660bf69b3eULL,
            0xe73c6710109675cdULL, 0xb55a3c96afcc8ea5ULL, 0xdff658ddca2d79f4ULL, 0x63f45b0eb5308381ULL, 0x6eb754df5456c0bbULL, 0x5bb842273760f490ULL, 0xff82ad808391824fULL, 0x2a841df9a7bbde20ULL,
            0x63d2c1f90bc689bcULL, 0x7f55b18a57f32320ULL, 0xf71029e8762a4d3dULL, 0x2fe8db31fd2f31a7ULL, 0x9dcdd2e1873da49fULL, 0xa7d5ccfd4bcbf1d0ULL, 0xb51bde1ce3df40baULL, 0xe1636a39da6a9291ULL,
            0x8eb73174af31883cULL, 0x8db74bd5b38f780cULL, 0x3cadac27ad1ea051ULL, 0x949679c41fff1818ULL, 0x29cc27dfd3d22420ULL, 0x0909ad1ea115e878ULL, 0x6eb8e42e16876e40ULL, 0x5fe94854bfcb5d05ULL,
            0x448abe28dbbf37fdULL, 0xc3d0659feb1b09efULL, 0xa94323591b545e0eULL, 0x26805175a3f73785ULL, 0xa851112796c08845ULL, 0xb8b00e6d564c5027ULL, 0x9796bc89c652ea67ULL, 0x61af0b5e983edb9dULL,
            0x0effed1c0c420c0cULL, 0x9494d49b4270c66cULL, 0x0d9e205fc896f9feULL, 0x0f5755b147776b49ULL, 0x052e8e9534610939ULL, 0xd3054110ec63a925ULL, 0x5d8ea0b1810ef671ULL, 0x1321e9ce7053ac85ULL,
            0x857eb16757463164ULL, 0xf335e905a4b928b5ULL, 0x8f0196eeff270328ULL, 0x02a200dc25135537ULL, 0xaa237653616845a0ULL, 0x9f2d74f75890ad88ULL, 0x2788ed9fe57f1755ULL, 0xae235966fc90d5eaULL,
            0xc3cedcc5d9692b85ULL, 0x2f808e86c22ac539ULL, 0x09ee57f33cbf6be7ULL, 0x4f9ee43869b0a1e1ULL, 0xab80a53a8dcad371ULL, 0x2c9c17368faace5eULL, 0x805aee48253f8d07ULL, 0xb5a41562c5bab1a2ULL,
            0x2d136beba56760dfULL, 0x1fb82bad7d0fb80dULL, 0xd6ddc6e78a907d66ULL, 0x5b270758e98c2534ULL, 0x479c52ced803652fULL, 0x2ef1f265ef7021b3ULL, 0x21f9ab56c927b45aULL, 0xaa8c897aa82f1117ULL,
            0x106ae43a657a8f7dULL, 0x51a9fb461be0f948ULL, 0x0298eba4ad0c04bcULL, 0x6720d721702cea17ULL, 0x28ef0dcc5aeb58dbULL, 0xa34e36fb3128944dULL, 0xd12ee75fc87fa8a6ULL, 0x6b967894729e54b8ULL,
            0xb0d003f454b9e88fULL, 0x555f50d958068564ULL, 0x416d32151b4f087cULL, 0x24df116a3e96bfc3ULL, 0x9cf5b132eda2fa0bULL, 0xd31dfa747586f729ULL, 0xa66efcdddb09040fULL, 0xcea21a80aaef613fULL,
            0x511f3bdc093145e9ULL, 0xaeceaa076f32d548ULL, 0x618d7fcf8eefec68ULL, 0xfe5cba6b7385f13dULL, 0x230e54c7ff01cba9ULL, 0x629091c61be0d282ULL, 0x818023153605ccaaULL, 0xd0afa09312f74a3bULL,
            0xbe1bbc53f563f2c6ULL, 0xea54cfe1dbf6fa2cULL, 0x2b61beac2fb92063ULL, 0x71f4ec90702a5fa5ULL, 0x93b62d3cc09b3fc6ULL, 0x7f02261c710955c0ULL, 0xc4f668214980cab5ULL, 0x1a219374f1999dabULL,
            0x84ab84dca9a877d1ULL, 0x454f2f3e8cd5b131ULL, 0x21a55f25660e768bULL, 0x2604c13b7d7aa847ULL, 0xae2c09c20656b92bULL, 0x4ddce65fa44631c9ULL, 0x0eef600ed4ba797dULL, 0x612d56b41a8d3d80ULL,
            0xf4801f72b0f8a9f3ULL, 0xb4752a2ae9b808bcULL, 0xa7014f58574dd2fdULL, 0x3e288d3628339421ULL, 0x3044db55deef029eULL, 0x722acfe94ba78e8eULL, 0x0eb58649b171ad87ULL, 0x3b535765177f5e6aULL,
            0x9150a82723f99979ULL, 0x32ec7e40407336f3ULL, 0x0ee05a131f2cedf7ULL, 0x199620ecda0fe067ULL, 0xef710a6f0141374eULL, 0x9b07824e21310dedULL, 0x5be64247dc49fa49ULL, 0xd7b2b158b18d49faULL,
            0x4d9d9ee857709c58ULL, 0xc32f6f9934f99ed4ULL, 0x8dc8ae033913bdb4ULL, 0x1ce99077c28ad553ULL, 0x731bef5b3a838592ULL, 0x00fbf46eb843f750ULL, 0xc3c90e449dae4c95ULL, 0xcbe1f444e5c08389ULL,
            0x20ca5d32cd19ecdbULL, 0x1fdd3dcb7eaba3fcULL, 0x3f706049d825b906ULL, 0x2becc567e1f51106ULL, 0x8a229f3af20a1c4bULL, 0x4161ae30ca4cc02eULL, 0x6c1e24c7f5dea76fULL, 0x5dd227fbf3e5f50eULL,
            0x649b8572f7719676ULL, 0x60fb1c93f77bd775ULL, 0xb67754113fdb81b6ULL, 0xd0eb3d8907f549e8ULL, 0x0487a9a08c275453ULL, 0x4aea133d004533c2ULL, 0x2ad3cb08098e5c22ULL, 0xe08d3f1188d73102ULL,
            0x654fc70b9f355f53ULL, 0xb11e2ccd2d9ae827ULL, 0x13fdb82050ab9c20ULL, 0x3f7fd8236b895275ULL, 0xeaf73c21656e8cc4ULL, 0xd521b91f02aaa36cULL, 0x50885fc5cb5579c0ULL, 0x3e916d46bc63cb86ULL,
            0x85f4d3ee2ceab969ULL, 0x8b77f42d63bc4e8bULL, 0x72a0b0d705c83b6cULL, 0xcef85dd65de5fc67ULL, 0xcfaf632fcbf8ddebULL, 0x249c83e481e60b16ULL, 0xe069c8a3820a969eULL, 0x739903f62e31cf53ULL,
            0x75f04a6dd230ad6aULL, 0xdde25a7afe8f91d1ULL, 0xe7d4bd28769bc796ULL, 0xbc67465942a97d13ULL, 0x014f43d495cd84edULL, 0xa200732338c28c2aULL, 0x0affc7b8dd4e4c5aULL, 0xd9c4ea221d2782eaULL,
            0xbd9cbfc05e1f5d76ULL, 0xe31811d12e83e932ULL, 0x801794d58029d439ULL, 0xe1f4fbbaa1820495ULL, 0x3d4f7a5578182359ULL, 0xc7edb6537ef571c5ULL, 0xa054287108b372bbULL, 0xb58fe32498670eeeULL,
            0x5551e310c6f65af5ULL, 0x9561e2f871f97396ULL, 0xf6d4075cb756474dULL, 0x55990d95cecca9cdULL, 0x81e1d31434fc66c4ULL, 0x56972f80aa352129ULL, 0x5840fd3fa21b7dcaULL, 0x4c434a862ed34e9dULL,
            0xa2bd25a329869dbcULL, 0xd4710544fee1b2a2ULL, 0x344ff3805419764bULL, 0xa44dadd867cb1345ULL, 0xba2c8cc2f725f592ULL, 0xcafc6b5cc2f0d032ULL, 0x195d22ebbbe1ca01ULL, 0x4237857765ad0f5cULL,
            0xf24e8ffaa13aa153ULL, 0x766b058816788e58ULL, 0x802231acfcc1881dULL, 0x1391acd0adfb373eULL, 0x049f1f148ce3b52bULL, 0xb9d109e190f2c693ULL, 0xa06d3a687872107fULL, 0x68b3ecae7271d06cULL,
            0xc808ef9579eaed7eULL, 0x917010a9e48eb160ULL, 0x957e1f19fe45e814ULL, 0xb5dac1d8d1c7a9aeULL, 0xe424aba6634cfaa6ULL, 0x029e63974c48c8edULL, 0x924303ecc6c03bdaULL, 0x60eab21aace7dd93ULL,
            0xa655c4ec4929b35dULL, 0xfe31bc64840792e2ULL, 0x2b34eec7050b1b5bULL, 0x790a0208b76543c4ULL, 0x1c31d62f4895bc83ULL, 0x566a46b8d1ff4393ULL, 0xf8ccac5237c1daceULL, 0x7bbc200071e52033ULL,
            0xc835b80b08cf415eULL, 0x124a9d97003ca3d9ULL, 0xd12813b2801391e7ULL, 0x49df09f619dd4208ULL, 0x46e39bb6a0c1ad78ULL, 0x6a04240b985e613fULL, 0xeaeff97fafb0f83dULL, 0x4d45587c7c7ef6feULL,
            0x6016c810be1288f5ULL, 0x1c1a43275632dd89ULL, 0x4db3bdae9bd522e0ULL, 0x4949dfc8a221f744ULL, 0xae4b155a7795df1cULL, 0x976ac615b7031040ULL, 0x3aefc6ffe495e6ecULL, 0x3bbf589afd6c44e3ULL,
            0xcbedf45c806d2768ULL, 0x41b24b336a6ff8f8ULL, 0xf88c6aa9737102c9ULL, 0xd45c53e500475432ULL, 0x343ccc9af99cce49ULL, 0x51e74b76f7e67498ULL, 0x98ef8846e6d1e83cULL, 0x9733b65bc7f9511eULL,
            0x6e226ccf931fd5ffULL, 0xf66c6e28ef32ebc6ULL, 0x9b8979f1a7e76f3cULL, 0x4e810286d2e3ec32ULL, 0x0ef19a16b1c70dc7ULL, 0x37f8cb3f5e6dad0bULL, 0x58c9d3f69eb37b0fULL, 0xb5393b86467b81e4ULL,
            0xd0a910c492986cadULL, 0xc8319f185a0be3e4ULL, 0x8831a9df63ae175cULL, 0x34859ca267943facULL, 0x8855f6935a49cfd6ULL, 0x0d472e552443dc6aULL, 0xa944704f2cfa85c4ULL, 0xacfc8efc26c15923ULL,
            0x3335977453e3e200ULL, 0x94f30b27b18678ecULL, 0x4cb21cddd914fd20ULL, 0xb28abcd7c13417deULL, 0xde2957825eea9ccdULL, 0xeb761178671c7ef0ULL, 0xc53ffa3bd1c48670ULL, 0xbe474c254e35076cULL,
            0x0d57c44c5d38e472ULL, 0x5dfccb97ed8e3f4aULL, 0x9406037e11dfcb3bULL, 0x07f5294a3a590621ULL, 0x65009a81b01c64d9ULL, 0xce17ded9bd28ed1fULL, 0xbf8f2d620ed440bcULL, 0x656b92a5e1e36a62ULL,
            0x6102a0ac5e1e47c2ULL, 0x5874a7f40023a754ULL, 0x402ef738ad5537abULL, 0x1917c067eba81b46ULL, 0xe955cd2fb3547e88ULL, 0x559915c195d9a058ULL, 0xacafc744bbaa9bf7ULL, 0xc298fe00d1c1f330ULL,
            0xd18b582478ea87c9ULL, 0x07bcd71ec6f2d3e7ULL, 0x5151d4066e99b237ULL, 0x1465123af73062ebULL, 0xdfb7d8130d76de87ULL, 0x2fca0f1dd53bbcd8ULL, 0xe9178afad30ae883ULL, 0xa48bb1d5148e46a4ULL,
            0xc11da5cad9cef51fULL, 0xd5c40530ff616c46ULL, 0x13f3f96f2c2e8ab1ULL, 0x68adff2dbd21c093ULL, 0x7d9c9af86c300642ULL, 0xfa811ffd633e6d07ULL, 0xbcf192eb32df776dULL, 0xde0c34b24e727c68ULL,
            0xc87d2151f26bb622ULL, 0xeecd233b6f856183ULL, 0x34bbd346b41a5502ULL, 0x6cc65625d7c275f5ULL, 0xb3bdf1882092999cULL, 0x928aa61c757308cbULL, 0xa24f5d23ca6e58e2ULL, 0x89af6634b219f9beULL,
            0x887fe1396eb40a89ULL, 0xe986604680a76a68ULL, 0x7246814c3d183cf0ULL, 0x08ce8a732c7de033ULL, 0xcafecbe66c1cd814ULL, 0xdd725885e4532df2ULL, 0x99b2107b721b557dULL, 0x0680475de3bce690ULL,
            0xa88e81c5e6315620ULL, 0x50ff95c61e1e5471ULL, 0x7527093244d6012eULL, 0x04b7578ccc9e0dd4ULL, 0xf374427910e7e31bULL, 0x4752169c5dbf253dULL, 0xcda00fbd9df4ecb2ULL, 0x43e3cd78e1522e7eULL,
            0x1fd209cc3f59dc30ULL, 0x1d3d8d6d40428ee5ULL, 0xd6ff0a4b24172425ULL, 0x67ad53b64ae0b568ULL, 0xdc7214957b900463ULL, 0x4d82434e08d33031ULL, 0x549ed64ba0ca3bc0ULL, 0xad0e81111b1a1852ULL,
            0x525a8c0020865ee4ULL, 0xe261578c8e6c3da9ULL, 0x1d44a80b585652b1ULL, 0xb2b81477a6dfb1bdULL, 0xf2b0836090909936ULL, 0xc399758dc6c6237eULL, 0xb7baabaa5887fa61ULL, 0x56077f0734474438ULL,
            0x7e9312855ef52e45ULL, 0xbb9d44f8f8e321a2ULL, 0x63a776edd0c65200ULL, 0xc23628f8e6bfb96cULL, 0xb2ec1553c284d982ULL, 0x930eda6ba071c8dbULL, 0xa09ce6b96bd368beULL, 0xc27e8072cf282f32ULL,
            0x46a758e21fe96465ULL, 0xace770e62ae70ba8ULL, 0x101a4e2cceb21e40ULL, 0x2d1157c6503a8016ULL, 0xd35033d2705a8b0eULL, 0xd2b1a2bd855e9a5bULL, 0xa72d99347165b4feULL, 0xfa23f8e5f5aa198dULL,
            0xd6ded97b2cf1b687ULL, 0xdd003ff1372b2fa9ULL, 0xca8cdae06a8f4cf0ULL, 0xbb8203ac835542cfULL, 0x8e5a66d8c98fd68eULL, 0xcd59813ec17c023fULL, 0x90891c7af164cebeULL, 0x258623a77081e10dULL,
            0xeb8a1f1e6de2627bULL, 0x90364221a40a26a8ULL, 0x37ee0f157eac8c63ULL, 0x9e3a13958d89f31aULL, 0x677edd55d21cb7cdULL, 0x769691437613c0aaULL, 0xa8897da25b989286ULL, 0x97674cd20f660640ULL,
            0x7d75733d14f34cadULL, 0x7e260f070209159eULL, 0x9921468251f1ad7dULL, 0xc36c1cad6fd30595ULL, 0xe4de39017ddb33acULL, 0xca2420c1e2c850b2ULL, 0x4fa74f45298f52aaULL, 0x91e8b42a5ff8ca47ULL,
            0x71e565cd3eaf6b8bULL, 0xf5f08259e12493b3ULL, 0x3589bde23b94ada6ULL, 0xac1358ca38acc390ULL, 0xb01ae0bba63024e6ULL, 0xb10a1e0ae95d8a1dULL, 0x4f5128a892f4c5e0ULL, 0xf14ba66ddd518e12ULL,
            0xbcc8f2d547973117ULL, 0xce28a3074cd148e9ULL, 0xfdacb07ada59e87fULL, 0x139ddef575b3197cULL, 0xd5101d35ca73e0cfULL, 0xc897d33285bdb73aULL, 0xe3907a11eab4bd56ULL, 0xb8eb337ac96fb152ULL,
            0x00410f0573e9001fULL, 0xe9ee1b7be918a9c4ULL, 0xbc92b84c7d4837c6ULL, 0x43e837caa179f19dULL, 0x97a8196e1d8b38c0ULL, 0xf3a2ffa397609f9cULL, 0x6c00ceb9d900fe3aULL, 0x31e17a85240e8b73ULL,
            0xfbe704acc6339414ULL, 0x2c9922f93b8557f7ULL, 0x156fa88cdac85c86ULL, 0x711c8fe49efbc822ULL, 0x6496f01c75652a82ULL, 0xb4a8c30cffb3c30cULL, 0xa96c7958be530846ULL, 0xa7bbc96efbbb4b03ULL,
            0xe2f653d00cbbb263ULL, 0xb307cd1c47b9015dULL, 0x7a74a13353b30b4eULL, 0xec70d78099c7f672ULL, 0x573a5bd1535e4241ULL, 0x7f04511168b1a9d0ULL, 0x7fb3931e2f889650ULL, 0x56d3d83f0ec79ff4ULL,
            0xbae17e8f26688b8fULL, 0xdf0502c39b9ba4b9ULL, 0x7b742385e710cfb8ULL, 0xc59c3bc4ea625c66ULL, 0xcd97d03ce7fee03cULL, 0x07305c16d82b7eaaULL, 0xd998008226548956ULL, 0xfd387b154689aaf9ULL,
            0x6017152d69c55269ULL, 0x60b9adb83fdc8eb3ULL, 0x88c3f83fdc8dee47ULL, 0x93873923bf6a1baaULL, 0x8d7a8100507a60b2ULL, 0x973aed26199087eaULL, 0x9df277ef68eff066ULL, 0xf3e0803ada1a822fULL,
            0x20cdf9c79aae4c40ULL, 0x356c1f8ebfef9470ULL, 0xdaeb551bcfcd9ef9ULL, 0x5dcdf0fa945f8945ULL, 0x2cd1d30a0af085a7ULL, 0xd6ccf07d776cac22ULL, 0xd41f4b773652a0afULL, 0x54e6dfcf6bdb4c86ULL,
            0xd251fe0b80317848ULL, 0x8692d077642a51beULL, 0x975b2e0e857757dcULL, 0x631180c4e2fde9deULL, 0x9c720fa4376f805eULL, 0xaaff7a2a4cf0390aULL, 0xc864b736acd452e8ULL, 0xb2abf60bcc4f267bULL,
            0x82346503949a997eULL, 0x3433bdfe6247342eULL, 0xa8f496f1d1029e21ULL, 0x44f6a006d80349a1ULL, 0x71a5c6a15dba8cf9ULL, 0xcc5d6acea2fdd586ULL, 0x9821c9def660bb65ULL, 0x3d71c1e046e33043ULL,
            0x15001582fa9e740fULL, 0x0ec25eb1feacfbddULL, 0x72a3fe6cd6e0fc68ULL, 0x4d58e0684079381cULL, 0xb5e512f11b209bcaULL, 0x3043a97f011ab255ULL, 0x77c0f8ef58f5438cULL, 0x6f14017edf8ef502ULL,
            0x46d64d141a4f3b63ULL, 0x776e5c74f2ad2991ULL, 0x8776e427fb8787a9ULL, 0x2e4740bc241fa7d9ULL, 0x603b06d7578f1fd8ULL, 0xe601c0fd09dd4fe6ULL, 0xfcda82ce272faa8fULL, 0xe3a57e04e96ace83ULL,
            0xe2c67a906dc58756ULL, 0x4f4d8b9c22e724beULL, 0x09a9cb4cd310bd50ULL, 0x9d76d34c8b7f515dULL, 0xabb2e6daa56236bcULL, 0x58da1d3effaaaad1ULL, 0x1bdb820a52e29207ULL, 0x6102dcedde090982ULL,
            0xa2276caafcb21994ULL, 0x40eed82e107b7cfbULL, 0x92f1636b9cf57c08ULL, 0xdb9bace6acc40d9cULL, 0xe1f705584ff6fee8ULL, 0xb21ec3cfd9a13d67ULL, 0x2b7e2facf656f5baULL, 0x43b8bf6a31ef6eedULL,
            0x067a09a3bd0b523cULL, 0x276455426c38e627ULL, 0xf9e5b081fdae3e6aULL, 0x74652614ef6dd95fULL, 0xe795afe9a21782c9ULL, 0x6162c6ae3b074efcULL, 0x1a0d74dde6404648ULL, 0x57ef16abc5728da5ULL,
            0x5cbcee19efe87673ULL, 0x9c79637b5866509cULL, 0x6bddee0fd546503fULL, 0x484156e39733e74aULL, 0x0857a90d03ca3d97ULL, 0x7d73a73c662f83d9ULL, 0x1803f2724a4316beULL, 0xf575a6edb8559b9cULL,
            0x13052360a752bde7ULL, 0x08976334b37f0420ULL, 0x2e50d2708f5e75fbULL, 0xfc23f71d6d205de6ULL, 0xb3233a5fb8954b03ULL, 0x5a87f7f2c3d30b2cULL, 0x6adc060f05226868ULL, 0xdd146a4a2af0265dULL,
            0x70abb791b5b6e162ULL, 0xf7051a0cb2e2f32fULL, 0x276825571a9f3894ULL, 0x90ed79e96226a738ULL, 0x07b598a8a2afe487ULL, 0x9475377ef32302b2ULL, 0xf368db5189b07d7aULL, 0xca59c80be6d76209ULL,
            0x4ac06324e93acca5ULL, 0xfe0a630ad6d79825ULL, 0x46653beb81bdeb81ULL, 0x181e834f295dcdecULL, 0xa1de7b4b6753d2a3ULL, 0x8ee0bc5db433bfbbULL, 0x32490e4a9b9f0bb4ULL, 0x64bdce067687eaa9ULL,
            0xcf66376755886b73ULL, 0xa1de4ef50bc5818aULL, 0xb02ecf90976c8fc1ULL, 0xb8cb31eff25ce26eULL, 0x2138bb540a306a56ULL, 0x3de2f1b171d06a06ULL, 0xe4bb070d021fb0f8ULL, 0xd631705987596c99ULL,
            0xfd93de62bc9bda27ULL, 0xb675ef78d84f5412ULL, 0x01cc14e0453c5001ULL, 0x1184a3f66a3b2e91ULL, 0x5f0c1dc7fba48acfULL, 0x5f23ed6053dff571ULL, 0x0cda45bf57666353ULL, 0xe799ce0d573a7c89ULL,
            0x0d284e9a2db55efbULL, 0x0dbbafbbb1b1bdd6ULL, 0x6b3369b1cbd0b23bULL, 0x8e76d812e6c61152ULL, 0xa6a036a556d6033cULL, 0x8434df7dbfae8c5aULL, 0xc76b456d68f7a9c2ULL, 0xd9c79ff3537e4f92ULL,
            0xf58b50b196e6feaaULL, 0xf1a8ff68fa1b3cbbULL, 0x20b44d29fd95af65ULL, 0xe39b890988c0d8bbULL, 0x5d59b3f0cea650b1ULL, 0xd2d0c7a71ae733f1ULL, 0x73b84a29e4e0dd36ULL, 0x4a068dc22cc78f40ULL,
            0x9ad90e3fb5ae862dULL, 0x7fea00e61cf5200cULL, 0x8590c15414c664f6ULL, 0x62af7b6086d927beULL, 0xc4b07c294be5ddeaULL, 0xd0b9cb570191bbe5ULL, 0x610022a634952659ULL, 0x391b54ff66e74e87ULL,
            0xacb8a06135a2dba5ULL, 0x325f1665df66948cULL, 0x58b2fd20326c9a0cULL, 0x89d27fe2887ec154ULL, 0x21ffe7cc02ab54ddULL, 0xad1df854bf4d5332ULL, 0xc384f89e145f10dbULL, 0x9c97a94a61095bcfULL,
            0xf6288a03d48c6478ULL, 0x62e11e670fd96eb0ULL, 0x4577b7177a1ed554ULL, 0xcc01c22c015071f7ULL, 0x52f167dbeceddd47ULL, 0x34ed3117aeddb2c9ULL, 0x2c08a15b3d4431b4ULL, 0x06444d91d2fd144dULL,
            0xa13b771d59908ef8ULL, 0xe5fc2d76ec10b72aULL, 0x6fb48d64e48cafc1ULL, 0x19155a3fa1c4d954ULL, 0x77cbbec42f791972ULL, 0xf7bed8fada17ae20ULL, 0x2cbf064a6e60ecb3ULL, 0xfe3253f558fa6285ULL,
            0x92a3ba679b0e4443ULL, 0xdfe8d49791673b5dULL, 0x96fd611044092f62ULL, 0xca97fe7904ba23edULL, 0x38805765cbd65d0dULL, 0x2cb5fe6143460e42ULL, 0xc9d7ebeff0c303f7ULL, 0xec16da89ae02474cULL,
            0xea7fc8b0f77e2bbdULL, 0x8939700585009117ULL, 0x50d958dd101aec06ULL, 0x5d578314353ca56cULL, 0x16a34a14db8c70b4ULL, 0xdcfbf6884f2f6fc7ULL, 0x331155e3fb352e32ULL, 0x5ed243df9a948d5cULL,
            0xbc8ac922c6f78b7eULL, 0x10736cd40b1d997aULL, 0x954bd2c449bbd149ULL, 0xd2bd4263c6b0411dULL, 0xafad97f4f8bc7556ULL,
        };

        static constexpr std::array<Hash, 16> m_castlingKey = {0,
                                                               RANDOM_ARRAY[768],
                                                               RANDOM_ARRAY[768 + 1],
                                                               RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 1],
                                                               RANDOM_ARRAY[768 + 2],
                                                               RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 2],
                                                               RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2],
                                                               RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2],
                                                               RANDOM_ARRAY[768 + 3],
                                                               RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 3],
                                                               RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 3],
                                                               RANDOM_ARRAY[768] ^ RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 3],
                                                               RANDOM_ARRAY[768 + 3] ^ RANDOM_ARRAY[768 + 2],
                                                               RANDOM_ARRAY[768 + 3] ^ RANDOM_ARRAY[768 + 2] ^ RANDOM_ARRAY[768],
                                                               RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2] ^ RANDOM_ARRAY[768 + 3],
                                                               RANDOM_ARRAY[768 + 1] ^ RANDOM_ARRAY[768 + 2] ^ RANDOM_ARRAY[768 + 3] ^ RANDOM_ARRAY[768]};

    public:
        static void generate();

        static Hash enpassantKey(File file) {
            return RANDOM_ARRAY[768 + 4 + static_cast<int>(file)];
        }

        static Hash sideKey() {
            return RANDOM_ARRAY[768 + 4 + 8];
        }

        static Hash castlingKey(int castling) {
            return m_castlingKey[castling];
        }

        static Hash castlingIndex(int index) {
            return RANDOM_ARRAY[768 + index];
        }

        static Hash pieceKey(Piece piece, Square square) {
            return RANDOM_ARRAY[static_cast<int>(piece) * 64 + static_cast<int>(square)];
        }

    }; // namespace zobrist

    inline void zobrist::generate() {
        std::random_device rd;
        std::mt19937_64    rng(rd());

        std::uniform_int_distribution<Hash> dist;

        std::cout << "static std::array<Hash, 781> RANDOM_ARRAY = {\n";

        for (int i = 0; i < 781; i++) {
            printf("0x%016llxULL, ", dist(rng));

            if (i % 5 == 0) {
                std::cout << "\n";
            }
        }

        std::cout << "};\n";
    }

} // namespace chess