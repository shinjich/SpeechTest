#include <windows.h>
#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Translation;

void SpeechContinuousTranslation()
{
	// サブスクリプションキーとサービスリージョンで Speech 構成のインスタンスを作成します
	// 下記の自分のサブスクリプションキーとサービスリージョンに編集しましょう (例 "XXXXX"->"japanwest")
	auto config = SpeechTranslationConfig::FromSubscription( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "XXXXX" );

	// 認識させたい言語を指定します (日本語="ja-JP", 英語="en-US")
	auto fromLanguage = "ja-JP";
	config->SetSpeechRecognitionLanguage( fromLanguage );

	// 上記の設定からスピーチ翻訳を作成します (ドイツ語="de", フランス語="fr", 中国語="zh" など)
	config->AddTargetLanguage( "en" );

	auto recognizer = TranslationRecognizer::FromConfig( config );

	cout << u8"何か話してみてください\n";

	// 認識終了の同期を担保します
	promise<void> recognitionEnd;

	// イベントに申請します
	recognizer->Recognizing.Connect( []( const TranslationRecognitionEventArgs& e )
	{
		for( const auto& it : e.Result->Translations )
		{
			cout << u8"\r翻訳中.. '" << it.first.c_str() << u8"': " << it.second.c_str() << u8"  " << std::flush;
		}
	});

	recognizer->Recognized.Connect( []( const TranslationRecognitionEventArgs& e )
	{
		if ( e.Result->Reason == ResultReason::RecognizedSpeech )
		{
			cout << u8"\n認識結果: " << e.Result->Text << u8" (翻訳できませんでした)" << std::endl;
		}
		else if ( e.Result->Reason == ResultReason::NoMatch )
		{
			cout << u8"\nNOMATCH: 声が認識できませんでした" << std::endl;
		}

		for( const auto& it : e.Result->Translations )
		{
			cout << u8"\r翻訳完了 '" << it.first.c_str() << u8"': " << it.second.c_str() << u8"  " << std::endl;
		}
	});

	recognizer->Canceled.Connect( [&recognitionEnd]( const TranslationRecognitionCanceledEventArgs& e )
	{
		cout << u8"\nCANCELED: 理由=" << (int) e.Reason << std::endl;
		if ( e.Reason == CancellationReason::Error )
		{
			cout << u8"CANCELED: エラーコード= " << (int) e.ErrorCode << std::endl;
			cout << u8"CANCELED: エラー詳細= " << e.ErrorDetails << std::endl;
			cout << u8"CANCELED: サブスクリプション情報を確認しましょう" << std::endl;

			// 認識停止を通知します
			recognitionEnd.set_value();
		}
	});

	// 連続的な認識を開始します (停止するには StopContinuousRecognitionAsync() を使います)
	recognizer->StartContinuousRecognitionAsync().get();

	cout << u8"Enter キーを押すと停止します\n";
	cin.get();

	// 認識を停止します
	recognizer->StopContinuousRecognitionAsync().get();
}

int wmain()
{
	SpeechContinuousTranslation();

	return 0;
}
