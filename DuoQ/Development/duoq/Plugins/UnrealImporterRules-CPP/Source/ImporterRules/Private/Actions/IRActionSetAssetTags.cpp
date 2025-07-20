/* MIT License
 *
 * Copyright (c) 2023 Ryan DowlingSoka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and//or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Actions/IRActionSetAssetTags.h"

#ifdef WITH_EDITOR
	#include "Subsystems/EditorAssetSubsystem.h"
#endif

bool UIRActionSetAssetTags::Apply_Implementation(UFactory* Factory, UObject* CreatedObject)
{
#ifdef WITH_EDITOR
	if (IsValid(CreatedObject))
	{
		if (UEditorAssetSubsystem* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>())
		{
			for (TTuple<FName, FString>& AssetTag : AssetTags)
			{
				if (AssetTag.Key == NAME_None || AssetTag.Value.IsEmpty())
					continue;

				EditorAssetSubsystem->SetMetadataTag(CreatedObject, AssetTag.Key, AssetTag.Value);
			}
			return true;
		}
	}
	return false;
#endif
}
