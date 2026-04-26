#include <windows.h>
#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Translation;

void SpeechTranslator()
{
	// サブスクリプションキーとサービスリージョンで Speech 構成のインスタンスを作成します
	// 下記の自分のサブスクリプションキーとサービスリージョンに編集しましょう (例 "XXXXX"->"japanwest")
	auto config = SpeechTranslationConfig::FromSubscription( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "XXXXX" );

	// 認識させたい言語を指定します (日本語="ja-JP", 英語="en-US")
	config->SetSpeechRecognitionLanguage( "ja-JP" );

	// 上記の設定からスピーチ翻訳を作成します (ドイツ語="de", フランス語="fr", 中国語="zh" など)
	config->AddTargetLanguage( "en" );
	auto recognizer = TranslationRecognizer::FromConfig( config );
	cout << u8"翻訳元の言語で話してください\n";

	cout << u8"何か話してみてください\n";

	auto result = recognizer->RecognizeOnceAsync().get();

	// 結果を確認します
	if ( result->Reason == ResultReason::TranslatedSpeech )
	{
		char pszTextUTF8[4096];
		for( const auto& it : result->Translations )
		{
			strcpy_s( pszTextUTF8, 4096, it.second.c_str() );
		}

		auto fromLanguage = "en-US";
		auto fromVoice = "AmberNeural";

		char szVoiceName[128];
		sprintf_s( szVoiceName, 128, "Microsoft Server Speech Text to Speech Voice (%s, %s)", fromLanguage, fromVoice );
		config->SetSpeechSynthesisLanguage( fromLanguage );
		config->SetSpeechSynthesisVoiceName( szVoiceName );
		auto audioOutput = Audio::AudioConfig::FromDefaultSpeakerOutput();

		// 上記の設定から音声合成を作成します
		auto synthesizer = SpeechSynthesizer::FromConfig( config, audioOutput );
		auto result = synthesizer->SpeakTextAsync( pszTextUTF8 ).get();
	}
	else if ( result->Reason == ResultReason::RecognizedSpeech )
	{
		cout << u8"認識結果: " << result->Text << u8" (翻訳できませんでした)" << std::endl;
	}
	else if ( result->Reason == ResultReason::NoMatch )
	{
		cout << u8"NOMATCH: 声を認識できませんでした" << std::endl;
	}
	else if ( result->Reason == ResultReason::Canceled )
	{
		auto cancellation = CancellationDetails::FromResult( result );
		cout << u8"CANCELED: 理由=" << (int) cancellation->Reason << std::endl;

		if ( cancellation->Reason == CancellationReason::Error )
		{
			cout << u8"CANCELED: エラーコード= " << (int) cancellation->ErrorCode << std::endl;
			cout << u8"CANCELED: エラー詳細= " << cancellation->ErrorDetails << std::endl;
			cout << u8"CANCELED: サブスクリプション情報を確認しましょう" << std::endl;
		}
	}
}

int main()
{
	SpeechTranslator();

	return 0;
}
