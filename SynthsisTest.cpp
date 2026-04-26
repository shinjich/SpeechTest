#include <windows.h>
#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

void SpeechSynthesis()
{
	// サブスクリプションキーとサービスリージョンで Speech 構成のインスタンスを作成します
	// 下記の自分のサブスクリプションキーとサービスリージョンに編集しましょう (例 "XXXXX"->"japanwest")
	auto config = SpeechConfig::FromSubscription( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "XXXXX" );

	// 合成させたい言語と声質を指定します (日本語="ja-JP", 英語="en-US") (七海="NanamiNeural", 圭太="KeitaNeural")
	// https://docs.microsoft.com/ja-jp/azure/cognitive-services/speech-service/language-support?tabs=speechtotext
	auto fromLanguage = "ja-JP";
	auto fromVoice = "KeitaNeural";

	char szVoiceName[128];
	sprintf_s( szVoiceName, 128, "Microsoft Server Speech Text to Speech Voice (%s, %s)", fromLanguage, fromVoice );
	config->SetSpeechSynthesisLanguage( fromLanguage );
	config->SetSpeechSynthesisVoiceName( szVoiceName );
	auto audioOutput = Audio::AudioConfig::FromDefaultSpeakerOutput();

	// 上記の設定から音声合成を作成します
	auto synthesizer = SpeechSynthesizer::FromConfig( config, audioOutput );

	// 下記の言葉をニューラル音声合成でしゃべってくれます。
	// 日本語以外をしゃべらせたい場合は fromLanguage と fromVoice を変更して、下記の言葉を書き換えてください
	auto pszTextUTF8 = u8"メディア情報論はとても楽しいのでやり遂げます。";

	// 文字化けしてしまいましたか？前回の講義資料を再確認してみましょう！
	cout << pszTextUTF8;
	auto result = synthesizer->SpeakTextAsync( pszTextUTF8 ).get();
}

int main()
{
	SpeechSynthesis();

	return 0;
}
